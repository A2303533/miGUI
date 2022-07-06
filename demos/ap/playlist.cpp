#include "framework/mgf.h"
#include "framework/ListBox.h"
#include "framework/Table.h"
#include "framework/TextBuffer.h"
#include "framework/Popup.h"
#include "framework/SystemWindow.h"
#include "framework/Unicode.h"
#include "ap.h"
#include "playlist.h"

#include <filesystem>
#include <fstream>

#include <ShlObj.h>
#include <shellapi.h>

extern AP_application* g_app;

void Playlist_popupCb_open(int id, struct mgPopupItem_s*);
void Playlist_popupCb_play(int id, struct mgPopupItem_s*);
void Playlist_popupCb_delete(int id, struct mgPopupItem_s*);
void Playlist_popupCb_showexpl(int id, struct mgPopupItem_s*);
int Playlist_onIsRowSelected(mgf::Table* tb, void* row);

TableTracklist::TableTracklist(mgf::Window* w, uint32_t colNum)	:mgf::Table(w, colNum){}
TableTracklist::~TableTracklist() {}
const mgUnicodeChar* TableTracklist::OnColTitleText(Table*, size_t* textLen, uint32_t colIndex)
{
	switch (colIndex)
	{
	case 0:
		*textLen = 7;
		return U"Playing";
	case 1:
		*textLen = 4;
		return U"File";
	case 2:
		*textLen = 1;
		return U"#";
	case 3:
		*textLen = 5;
		return U"Title";
	case 4:
		*textLen = 8;
		return U"Duration";
	}
	return 0;
}
int TableTracklist::OnDrawRow(Table*, void* row, uint32_t col, mgUnicodeChar** text, size_t* textlen)
{
	if (row)
	{
		static mgf::UnicodeString playingText;
		playingText.Assign(U"*", 1);

		PlaylistNode* nd = (PlaylistNode*)row;
		static mgf::UnicodeString textBuffer;
		// col is where you want to show text for current row
		switch (col)
		{
		case 0:
		{
			if (nd->m_isPlaying)
			{
				textBuffer.Clear();
				textBuffer.Append(playingText);
				*text = textBuffer.Data();
				*textlen = textBuffer.Size();
			}
			else
			{
				*text = 0;
				*textlen = 0;
			}
		}break;
		case 1:
			*text = nd->m_audioFilePath.Data();
			*textlen = nd->m_audioFilePath.Size();
			break;
		case 2:
			textBuffer.Clear();
			textBuffer.Append(nd->m_info_number.data(), nd->m_info_number.size());
			*text = textBuffer.Data();
			*textlen = textBuffer.Size();
			break;
		case 3:
			textBuffer.Clear();
			textBuffer.Append(nd->m_info_title.data(), nd->m_info_title.size());
			*text = textBuffer.Data();
			*textlen = textBuffer.Size();
			break;
		case 4:
			textBuffer.Clear();
			textBuffer.Append(nd->m_info_duration.data(), nd->m_info_duration.size());
			*text = textBuffer.Data();
			*textlen = textBuffer.Size();
			break;
		}

		return 1;
	}
	return 0;
}
void TableTracklist::OnRowClick(Table*, void* row, uint32_t rowIndex, uint32_t mouseButton, mgInputContext_s* input)
{
	if (mouseButton != 1)
		return;

	static PlaylistNode* oldNd = 0;

	PlaylistNode* nd = (PlaylistNode*)row;

	if (input->LMBClickCount == 2)
	{
		if (oldNd == nd)
		{
			g_app->m_playlistMgr->PlayTrack(nd);
		}
	}

	for (size_t i = 0, sz = nd->m_playlist->m_nodes.size(); i < sz; ++i)
	{
		nd->m_playlist->m_nodes[i]->m_isSelected = false;
	}
	nd->m_isSelected = true;

	oldNd = nd;
}
int TableTracklist::OnIsRowSelected(Table*, void* row)
{
	PlaylistNode* nd = (PlaylistNode*)row;
	return (int)nd->m_isSelected;
}

ListboxPlaylist::ListboxPlaylist(mgf::Window* w) :mgf::ListBox(w) {}	
ListboxPlaylist::~ListboxPlaylist() {}
mgUnicodeChar ListboxPlaylist::OnTextInputCharEnter(ListBox*, mgUnicodeChar c)
{
	bool good = false;

	if (iswalnum((unsigned short)c))
		good = true;

	if (!good)
	{
		if (c == L'_')
			good = true;
	}

	return good ? c : 0;
}

int ListboxPlaylist::OnTextInputEndEdit(ListBox*, int i, const mgUnicodeChar* str, void* item)
{
	/*
	* i:
	*	1 - Enter
	*   2 - click somewhere
	*   3 - Escape
	*/
	PlayList* pl = (PlayList*)item;

	if (str)
	{
		if (i == 1)
		{
			uint32_t len = mgUnicodeStrlen(str);
			if (len)
			{
				pl->m_name.Assign(str, len);
				pl->RenameFile();
			}
		}
	}

	return i;
}

int ListboxPlaylist::OnIsItemSelected(ListBox* e, void* item)
{
	PlayList* pl = (PlayList*)item;
	return (int)pl->m_isSelected;
}

void ListboxPlaylist::OnItemClick(ListBox* e, void* item, uint32_t itemIndex, uint32_t mouseButton)
{
	PlayList* pl = (PlayList*)item;
	if (mouseButton == 1)
	{
		pl->m_mgr->SelectPlaylist(pl);
	}
	else if (mouseButton == 2)
	{
		pl->m_mgr->ShowListboxPopup(pl);
	}
}

int ListboxPlaylist::OnDrawItem(ListBox*, void* item, uint32_t itemIndex, mgUnicodeChar** text, size_t* textlen)
{
	PlayList* pl = (PlayList*)item;

	*text = pl->m_name.Data();
	*textlen = pl->m_name.Size();

	return 1;
}

PlayList::PlayList()
{

}

PlayList::~PlayList()
{
	Free();
}

void PlayList::Free()
{
	for (uint32_t i = 0, sz = m_nodes.size(); i < sz; ++i)
	{
		delete m_nodes[i];
	}
	m_nodes.clear();
}

void PlayList::Save()
{
	FILE* f = _wfopen(m_filePath.data(), L"wb");
	if (f)
	{
		uint16_t bom = 0xFEFF;
		fwrite(&bom, 2, 1, f);

		std::wstring wstr;
		m_name.Get(wstr);

		fwrite(L"n:", 2 * sizeof(wchar_t), 1, f);
		if(wstr.size())
			fwrite(wstr.data(), wstr.size() * sizeof(wchar_t), 1, f);
		fwrite(L"\n", 1 * sizeof(wchar_t), 1, f);

		mgf::StringW strForNumbers;

		for (uint32_t i = 0, sz = m_nodes.size(); i < sz; ++i)
		{
			m_nodes[i]->m_audioFilePath.Get(wstr);

			fwrite(L"t:", 2 * sizeof(wchar_t), 1, f);
			if (wstr.size())
				fwrite(wstr.data(), wstr.size() * sizeof(wchar_t), 1, f);
			fwrite(L"\n", 1 * sizeof(wchar_t), 1, f);

			strForNumbers.clear();
			strForNumbers += m_nodes[i]->m_begin;

			fwrite(L"b:", 2 * sizeof(wchar_t), 1, f);
			fwrite(strForNumbers.data(), strForNumbers.size() * sizeof(wchar_t), 1, f);
			fwrite(L"\n", 1 * sizeof(wchar_t), 1, f);

			strForNumbers.clear();
			strForNumbers += m_nodes[i]->m_end;

			fwrite(L"e:", 2 * sizeof(wchar_t), 1, f);
			fwrite(strForNumbers.data(), strForNumbers.size() * sizeof(wchar_t), 1, f);
			fwrite(L"\n", 1 * sizeof(wchar_t), 1, f);
		}

		fclose(f);
	}
	this->m_mgr->SaveStateFile();
}

void PlayList::RenameFile()
{
	std::filesystem::path p(m_filePath.data());
	std::filesystem::path pP = p.remove_filename();
	
	std::wstring wstrFromUStr;
	m_name.Get(wstrFromUStr);

	mgf::StringW newFilePath = pP.c_str();
	newFilePath += wstrFromUStr.data();
	newFilePath += L".appl";
	newFilePath.flip_slash();

	if (m_filePath == newFilePath)
		return;

	newFilePath = g_app->m_playlistMgr->CheckPLName(&newFilePath);
	{
		std::filesystem::path p = newFilePath.data();
		std::filesystem::path fn = p.filename();

		m_name.Assign(fn.c_str(), wcslen(fn.c_str()));
		m_name.PopBackBefore('.');
		m_name.PopBack();
	}

	std::filesystem::rename(m_filePath.data(), newFilePath.data());

	m_filePath = newFilePath;
	m_fileName = std::filesystem::path(newFilePath.data()).filename().c_str();
	Save();
}

PlayListManager::PlayListManager()
{
	if (g_app->m_tableTracklist)
	{
		g_app->m_tableTracklist->GetColSizes()[0] = 45;
		g_app->m_tableTracklist->GetColSizes()[1] = 320;
		g_app->m_tableTracklist->GetColSizes()[2] = 20;
		g_app->m_tableTracklist->GetColSizes()[3] = 170;
		g_app->m_tableTracklist->GetColSizes()[4] = 50;
		g_app->m_tableTracklist->SetScrollSpeed(50.f);
		g_app->m_tableTracklist->SetRowHeight(20);
	}

	mgPopupItemInfo_s listboxPopupItems[] =
	{
		{0, U"Open", 0, Playlist_popupCb_open, mgPopupItemType_default, 0, 0, 1},
		{0, U"Play", 0, Playlist_popupCb_play, mgPopupItemType_default, 0, 0, 1},
		{0, 0, 0, 0, mgPopupItemType_separator, 0, 0, 1},
		{0, U"Delete", 0, Playlist_popupCb_delete, mgPopupItemType_default, 0, 0, 1},
		{0, 0, 0, 0, mgPopupItemType_separator, 0, 0, 1},
		{0, U"Show in explorer", 0, Playlist_popupCb_showexpl, mgPopupItemType_default, 0, 0, 1},
	};
	m_listBoxPopup = g_app->m_context->CreatePopup(listboxPopupItems, 6, true);


	IKnownFolderManager* pManager;
	HRESULT hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pManager));
	if (SUCCEEDED(hr))
	{
		UINT cCount;
		KNOWNFOLDERID* pkfid;

		hr = pManager->GetFolderIds(&pkfid, &cCount);
		if (SUCCEEDED(hr))
		{
			for (UINT i = 0; i < cCount; i++)
			{
				IKnownFolder* pKnownFolder;
				hr = pManager->GetFolder(pkfid[i], &pKnownFolder);
				if (SUCCEEDED(hr))
				{
					LPWSTR ppszPath = 0;
					pKnownFolder->GetPath(KF_FLAG_DEFAULT, &ppszPath);
					if (ppszPath && (pkfid[i] == FOLDERID_Music))
					{
						m_musicDir = ppszPath;
					//	wprintf(L": %s (%x16)\n", ppszPath, pkfid[i]);
					}

					/*IShellItem* psi;
					hr = pKnownFolder->GetShellItem(0, IID_PPV_ARGS(&psi));
					if (SUCCEEDED(hr))
					{
						hr = prf->AddItem(psi);
						psi->Release();
					}*/
					pKnownFolder->Release();
				}
			}
			CoTaskMemFree(pkfid);
		}
		pManager->Release();
	}

	if (!m_musicDir.size())
		throw "Can't locate user music dir";

	m_musicDir.flip_slash();

	m_playlistDir = m_musicDir;
	m_playlistDir += "/AP/";
	if (!std::filesystem::exists(m_playlistDir.data()))
		std::filesystem::create_directory(m_playlistDir.data());

	m_stateFilePath = g_app->m_framework->GetAppDir()->data();
	m_stateFilePath += L"ap.state";
	if (std::filesystem::exists(m_stateFilePath.data()))
	{
		mgf::TextBuffer textBuf;
		textBuf.FromFile(m_stateFilePath.data());
		mgf::StringW wstr;
		while (!textBuf.IsEnd())
		{
			textBuf.GetToken(&wstr, 0);
			if (std::wcscmp(wstr.data(), L"pl") == 0)
			{
				textBuf.GetToken(&wstr, 0);
				if (std::wcscmp(wstr.data(), L":") == 0)
				{
					textBuf.GetLine(&wstr);
					wstr.trim_spaces();
				}
				else
				{
					wstr.clear();
					textBuf.SkipLine();
				}
			}
			else
			{
				wstr.clear();
				textBuf.SkipLine();
			}

			if (wstr.size())
			{
				mgf::StringW plPath = m_playlistDir;
				plPath += wstr;
				if (std::filesystem::exists(plPath.data()))
				{
					LoadPlaylist(plPath.c_str());
				}
			}
		}

		// read pla:
		textBuf.ToBegin();
		while (!textBuf.IsEnd())
		{
			textBuf.GetToken(&wstr, 0);
			if (std::wcscmp(wstr.data(), L"pla") == 0)
			{
				textBuf.GetToken(&wstr, 0);
				if (std::wcscmp(wstr.data(), L":") == 0)
				{
					textBuf.GetLine(&wstr);
					wstr.trim_spaces();
				}
				else
				{
					wstr.clear();
					textBuf.SkipLine();
				}
			}
			else
			{
				wstr.clear();
				textBuf.SkipLine();
			}

			if (wstr.size())
			{
				for (size_t i = 0, sz = m_playlists.size(); i < sz; ++i)
				{
					if (m_playlists[i]->m_fileName == wstr)
					{
						SelectPlaylist(m_playlists[i]);
						break;
					}
				}
				break;
			}
		}
	}

	for (auto& entry : std::filesystem::directory_iterator(m_playlistDir.data()))
	{
		auto path = entry.path();
		if (path.has_extension())
		{
			auto ex = path.extension();
			if (ex == ".appl")
				LoadPlaylist(path.c_str());
		}
	}

	printf("%u playlists\n", m_playlists.size());

	SaveStateFile();
	_updateGUIListbox();
}

PlayListManager::~PlayListManager()
{
	for (uint32_t i = 0; i < m_playlists.size(); ++i)
	{
		delete m_playlists[i];
	}
}

mgf::StringW PlayListManager::CheckPLName(mgf::StringW* str)
{
	mgf::StringW s;

	for (int i = 0; i < 0xffff; ++i)
	{
		s = str->data();
		s.pop_back();
		s.pop_back();
		s.pop_back();
		s.pop_back();
		s.pop_back();

		if (i)
			s += i;

		if (s[s.size() - 1] == L'/')
			s += "_";

		s += ".appl";

		if (!std::filesystem::exists(s.data()))
			break;
	}

	return s;
}

void PlayListManager::LoadPlaylist(const wchar_t* pth)
{
	mgf::StringW strw = pth;
	for (size_t i = 0; i < m_playlists.size(); ++i)
	{
		if (m_playlists[i]->m_filePath == strw)
			return;
	}

	PlayList* newPL = new PlayList;
	newPL->m_mgr = this;
	newPL->m_filePath = pth;
	newPL->m_fileName = std::filesystem::path(pth).filename().c_str();

	std::filesystem::path p = pth;
	std::filesystem::path fn = p.filename();

	newPL->m_name.Assign(fn.c_str(), wcslen(fn.c_str()));
	newPL->m_name.PopBackBefore('.');
	newPL->m_name.PopBack();

	m_playlists.emplace_back(newPL);

	_updateGUIListbox();
}

void PlayListManager::AddNew()
{
	static uint32_t pl_number = 0;

	PlayList* newPL = new PlayList;
	newPL->m_mgr = this;
	newPL->m_name.Assign(U"Playlist", 8);
	newPL->m_name.Append(pl_number++);

	std::wstring wstrFromUStr;
	newPL->m_name.Get(wstrFromUStr);

	mgf::StringW plFilePath = m_playlistDir;
	plFilePath += wstrFromUStr.c_str();
	plFilePath += ".appl";

	plFilePath = CheckPLName(&plFilePath);
	{
		std::filesystem::path p = plFilePath.data();
		std::filesystem::path fn = p.filename();

		newPL->m_name.Assign(fn.c_str(), wcslen(fn.c_str()));
		newPL->m_name.PopBackBefore('.');
		newPL->m_name.PopBack();
	}


	newPL->m_filePath = plFilePath.data();
	newPL->m_fileName = std::filesystem::path(plFilePath.data()).filename().c_str();
	m_playlists.emplace_back(newPL);
	
	newPL->Save();
	SaveEditPlaylist();
	_updateGUIListbox();
	
	newPL->Save();
}

void PlayListManager::_updateGUIListbox()
{
	if(g_app->m_listboxPlaylist)
		g_app->m_listboxPlaylist->SetData((void**)m_playlists.data(), m_playlists.size());
}
void PlayListManager::_updateGUITable()
{
	if(g_app->m_tableTracklist)
		g_app->m_tableTracklist->SetData(0,0);
	if (m_editPlaylist)
	{
		if(m_editPlaylist->m_nodes.size())
			g_app->m_tableTracklist->SetData((void**)m_editPlaylist->m_nodes.data(), m_editPlaylist->m_nodes.size());
	}
}

void PlayListManager::SaveStateFile()
{
	mgf::StringA stra = m_stateFilePath.to_utf8();
	FILE* f = fopen(stra.data(), "wb");
	if (f)
	{
		uint16_t bom = 0xFEFF;
		fwrite(&bom, 2, 1, f);

		fwrite(L";pl - playlist\n", 1, sizeof(wchar_t) * 15, f);
		fwrite(L";pla - active (selected) playlist\n\n", 1, sizeof(wchar_t) * 35, f);

		for (uint32_t i = 0; i < m_playlists.size(); ++i)
		{
			fwrite(L"pl:", 1, sizeof(wchar_t) * 3, f);
			fwrite(m_playlists[i]->m_fileName.data(), 1, m_playlists[i]->m_fileName.size() * sizeof(wchar_t), f);
			fputwc(L'\n', f);
		}

		if (m_playPlaylist)
		{
			fwrite(L"pla:", 1, sizeof(wchar_t) * 4, f);
			fwrite(m_playPlaylist->m_fileName.data(), 1, m_playPlaylist->m_fileName.size() * sizeof(wchar_t), f);
			fputwc(L'\n', f);
		}
		else if (m_editPlaylist)
		{
			fwrite(L"pla:", 1, sizeof(wchar_t) * 4, f);
			fwrite(m_editPlaylist->m_fileName.data(), 1, m_editPlaylist->m_fileName.size() * sizeof(wchar_t), f);
			fputwc(L'\n', f);
		}

		fclose(f);
	}
}

void PlayListManager::SetEditPlaylist(PlayList* p)
{
	m_editPlaylist = p;
}

void PlayListManager::SelectPlaylist(PlayList* pl)
{
	// if m_editPlaylist not current played playlist
	//  then remove all nodes from m_editPlaylist
	if (m_editPlaylist)
	{
		if (m_editPlaylist != m_playPlaylist)
		{
			m_editPlaylist->Free();
			_updateGUITable();
		}
	}

	// select new 
	for (size_t i = 0, sz = m_playlists.size(); i < sz; ++i)
	{
		m_playlists[i]->m_isSelected = false;
	}
	pl->m_isSelected = true;
	m_editPlaylist = pl;
	SaveStateFile();

	//read file if m_editPlaylist not current played playlist
	if (m_editPlaylist)
	{
		if (m_editPlaylist != m_playPlaylist)
		{
			m_editPlaylist->Free();

			if (std::filesystem::exists(m_editPlaylist->m_filePath.data()))
			{
				mgf::StringW wstr;
				mgf::TextBuffer textBuf;
				textBuf.FromFile(m_editPlaylist->m_filePath.data());
				while (!textBuf.IsEnd())
				{
					textBuf.GetToken(&wstr, 0);
					if (std::wcscmp(wstr.data(), L"t") == 0)
					{
						textBuf.GetToken(&wstr, 0);
						if (std::wcscmp(wstr.data(), L":") == 0)
						{
							textBuf.GetLine(&wstr);
							wstr.trim_spaces();
						}
						else
						{
							wstr.clear();
							textBuf.SkipLine();
						}
					}
					else
					{
						wstr.clear();
						textBuf.SkipLine();
					}

					if (wstr.size())
					{
						//wprintf(L"ADD TRACK: %s\n", wstr.data());
						if (std::filesystem::exists(wstr.data()))
						{
							PlaylistNode * node = new PlaylistNode;
							node->m_playlist = m_editPlaylist;
							node->m_audioFilePath.Assign(wstr.data(), wstr.size());
							m_editPlaylist->m_nodes.push_back(node);
						}
					}
				}
			}
		}
	}
	_updateGUITable();
}

void PlayListManager::ShowListboxPopup(PlayList* pl)
{
	m_playlistOnPopup = pl;
	m_listBoxPopup->Show(g_app->m_context->m_input->mousePosition.x, g_app->m_context->m_input->mousePosition.y);
}

void Playlist_popupCb_open(int id, struct mgPopupItem_s*)
{
	g_app->m_playlistMgr->PopupOpenPlaylist();
}

void Playlist_popupCb_play(int id, struct mgPopupItem_s*)
{
	g_app->m_playlistMgr->PopupPlayPlaylist();
}

void Playlist_popupCb_delete(int id, struct mgPopupItem_s*)
{
	g_app->m_playlistMgr->PopupDeletePlaylist();
}

void Playlist_popupCb_showexpl(int id, struct mgPopupItem_s*)
{
	g_app->m_playlistMgr->PopupShowPlaylistInExplorer();
}

void PlayListManager::PopupOpenPlaylist()
{
	if (!m_playlistOnPopup)
		return;
	this->SelectPlaylist(m_playlistOnPopup);
	m_playlistOnPopup = 0;
}

void PlayListManager::PopupPlayPlaylist()
{
	if (!m_playlistOnPopup)
		return;
	this->SelectPlaylist(m_playlistOnPopup);
	m_playlistOnPopup = 0;
}

void PlayListManager::PopupDeletePlaylist()
{
	if (!m_playlistOnPopup)
		return;

	int res = MessageBox(g_app->m_context->GetSystemWindow()->GetOSData()->hWnd,
		L"Are you sure?", 
		L"Delete playlist", 
		MB_YESNO | MB_ICONQUESTION);
	if (res == IDYES)
	{
		DeletePlaylist(m_playlistOnPopup);
	}
	m_playlistOnPopup = 0;
}

void PlayListManager::PopupShowPlaylistInExplorer()
{
	if (!m_playlistOnPopup)
		return;
	mgf::StringW fp = m_playlistOnPopup->m_filePath;
	fp.flip_backslash();
	mgf::StringW cmd;
	cmd = L"/select,\"";
	cmd += fp.data();
	cmd += L"\"";
	ShellExecute(0, L"runas", L"explorer.exe", cmd.data(), 0, SW_SHOWNORMAL);
	m_playlistOnPopup = 0;
}

void PlayListManager::StopPlaying()
{

}

void PlayListManager::DeletePlaylist(PlayList* pl)
{
	std::filesystem::remove(pl->m_filePath.data());
	for (size_t i = 0, sz = m_playlists.size(); i < sz; ++i)
	{
		if (m_playlists[i] == pl)
		{
			m_playlists.erase(m_playlists.begin() + i);
			break;
		}
	}

	if (m_playPlaylist == pl)
	{
		StopPlaying();
		m_playPlaylist = 0;
	}

	delete pl;

	if (m_editPlaylist == pl)
		m_editPlaylist = 0;

	if(m_playPlaylist)
		m_editPlaylist = m_playPlaylist;

	_updateGUIListbox();
	_updateGUITable();
	SaveStateFile();
}

bool PlayListManager::IsSupportedFormat(const wchar_t* ext)
{
	if (std::wcscmp(ext, L".wav") == 0)
		return true;
	if (std::wcscmp(ext, L".mp3") == 0)
		return true;
	if (std::wcscmp(ext, L".flac") == 0)
		return true;
	if (std::wcscmp(ext, L".ogg") == 0)
		return true;
	if (std::wcscmp(ext, L".opus") == 0)
		return true;

	return false;
}

void PlayListManager::AddTrackToEditPlaylist(const wchar_t* fn)
{
	std::filesystem::path pth = fn;
	if (!std::filesystem::exists(pth))
		return;

	std::filesystem::path ext = pth.extension(); //return like this .asd
	if (!IsSupportedFormat(ext.c_str()))
		return;

	PlaylistNode* node = new PlaylistNode;
	node->m_audioFilePath.Assign(fn, wcslen(fn));
	node->m_playlist = m_editPlaylist;
	m_editPlaylist->m_nodes.push_back(node);
	SaveEditPlaylist();
	_updateGUITable();
}

void PlayListManager::SaveEditPlaylist()
{
	if (!m_editPlaylist)
		return;
	m_editPlaylist->Save();
}

void PlayListManager::PlayTrack(PlaylistNode* nd)
{
	if (nd->m_playlist == m_playPlaylist)
	{

	}
	else
	{
		StopPlaying();
		m_playPlaylist = nd->m_playlist;


	}
}