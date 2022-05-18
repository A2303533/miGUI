#include "framework/mgf.h"
#include "framework/ListBox.h"
#include "framework/Table.h"
#include "framework/TextBuffer.h"
#include "framework/Popup.h"
#include "framework/SystemWindow.h"
#include "ap.h"
#include "playlist.h"

#include <filesystem>
#include <fstream>

#include <ShlObj.h>

extern AP_application* g_app;

int Playlist_LB_onTextInputEndEdit(mgf::ListBox* lb, int i, const wchar_t* str, void* editItem);
wchar_t Playlist_LB_onTextInputCharEnter(mgf::ListBox* lb, wchar_t c);
int Playlist_LB_onIsItemSelected(mgf::ListBox* lb, void* item);
void Playlist_LB_onItemClick(mgf::ListBox* lb, void* item, uint32_t itemIndex, uint32_t mouseButton);
int Playlist_LB_onDrawItem(mgf::ListBox*, void* item, uint32_t itemIndex, wchar_t** text, uint32_t* textlen);
void Playlist_BTN_newPL_onRelease(mgf::Element* e);
void Playlist_popupCb_open(int id, struct mgPopupItem_s*);
void Playlist_popupCb_play(int id, struct mgPopupItem_s*);
void Playlist_popupCb_delete(int id, struct mgPopupItem_s*);
void Playlist_popupCb_showexpl(int id, struct mgPopupItem_s*);
const wchar_t* Playlist_onColTitleText(mgf::Table*, uint32_t* textLen, uint32_t colIndex);
int Playlist_onDrawRow(mgf::Table* tb, void* row, uint32_t col, wchar_t** text, uint32_t* textlen);
void Playlist_onRowClick(mgf::Table* tb, void* row, uint32_t rowIndex, uint32_t mouseButton, mgInputContext_s* input);
int Playlist_onIsRowSelected(mgf::Table* tb, void* row);

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

		fwrite(L"n:", 2 * sizeof(wchar_t), 1, f);
		fwrite(m_name.data(), m_name.size() * sizeof(wchar_t), 1, f);
		fwrite(L"\n", 1 * sizeof(wchar_t), 1, f);

		mgf::StringW strForNumbers;

		for (uint32_t i = 0, sz = m_nodes.size(); i < sz; ++i)
		{
			fwrite(L"t:", 2 * sizeof(wchar_t), 1, f);
			fwrite(m_nodes[i]->m_audioFilePath.data(), m_nodes[i]->m_audioFilePath.size() * sizeof(wchar_t), 1, f);
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

	mgf::StringW newFilePath = pP.c_str();
	newFilePath += m_name.data();
	newFilePath += L".appl";
	newFilePath.flip_slash();

	if (m_filePath == newFilePath)
		return;

	newFilePath = g_app->m_playlistMgr->CheckPLName(&newFilePath);
	{
		std::filesystem::path p = newFilePath.data();
		std::filesystem::path fn = p.filename();

		m_name = fn.c_str();
		m_name.pop_back_before(L'.');
		m_name.pop_back();
	}

	std::filesystem::rename(m_filePath.data(), newFilePath.data());

	m_filePath = newFilePath;
	m_fileName = std::filesystem::path(newFilePath.data()).filename().c_str();
	Save();
}

PlayListManager::PlayListManager()
{
	if (g_app->m_listboxPlaylist)
	{
		g_app->m_listboxPlaylist->onIsItemSelected = Playlist_LB_onIsItemSelected;
		g_app->m_listboxPlaylist->onItemClick = Playlist_LB_onItemClick;
		g_app->m_listboxPlaylist->onDrawItem = Playlist_LB_onDrawItem;
	}

	if (g_app->m_tableTracklist)
	{
		g_app->m_tableTracklist->GetColSizes()[0] = 45;
		g_app->m_tableTracklist->GetColSizes()[1] = 320;
		g_app->m_tableTracklist->GetColSizes()[2] = 20;
		g_app->m_tableTracklist->GetColSizes()[3] = 170;
		g_app->m_tableTracklist->GetColSizes()[4] = 50;
		g_app->m_tableTracklist->SetScrollSpeed(50.f);
		g_app->m_tableTracklist->SetRowHeight(20);
		g_app->m_tableTracklist->onColTitleText = Playlist_onColTitleText;
		g_app->m_tableTracklist->onDrawRow = Playlist_onDrawRow;
		g_app->m_tableTracklist->onRowClick = Playlist_onRowClick;
		g_app->m_tableTracklist->onIsRowSelected = Playlist_onIsRowSelected;
	}

	mgPopupItemInfo_s listboxPopupItems[] =
	{
		{0, L"Open", 0, Playlist_popupCb_open, mgPopupItemType_default, 0, 0, 1},
		{0, L"Play", 0, Playlist_popupCb_play, mgPopupItemType_default, 0, 0, 1},
		{0, 0, 0, 0, mgPopupItemType_separator, 0, 0, 1},
		{0, L"Delete", 0, Playlist_popupCb_delete, mgPopupItemType_default, 0, 0, 1},
		{0, 0, 0, 0, mgPopupItemType_separator, 0, 0, 1},
		{0, L"Show in explorer", 0, Playlist_popupCb_showexpl, mgPopupItemType_default, 0, 0, 1},
	};
	m_listBoxPopup = g_app->m_context->CreatePopup(g_app->m_popupFont, listboxPopupItems, 6);


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

	newPL->m_name = fn.c_str();
	newPL->m_name.pop_back_before(L'.');
	newPL->m_name.pop_back();

	m_playlists.emplace_back(newPL);

	_updateGUIListbox();
}

void PlayListManager::AddNew()
{
	static uint32_t pl_number = 0;

	PlayList* newPL = new PlayList;
	newPL->m_mgr = this;
	newPL->m_name = L"Playlist";
	newPL->m_name += pl_number++;

	mgf::StringW plFilePath = m_playlistDir;
	plFilePath += newPL->m_name;
	plFilePath += ".appl";

	plFilePath = CheckPLName(&plFilePath);
	{
		std::filesystem::path p = plFilePath.data();
		std::filesystem::path fn = p.filename();

		newPL->m_name = fn.c_str();
		newPL->m_name.pop_back_before(L'.');
		newPL->m_name.pop_back();
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

wchar_t Playlist_LB_onTextInputCharEnter(mgf::ListBox* lb, wchar_t c)
{
	bool good = false;

	if (iswalnum(c))
		good = true;

	if (!good)
	{
		if (c == L'_')
			good = true;
	}

	return good ? c : 0;
}

int Playlist_LB_onTextInputEndEdit(mgf::ListBox* lb, int i, const wchar_t* str, void* item)
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
			uint32_t len = wcslen(str);
			if (len)
			{
				pl->m_name = str;
				pl->RenameFile();
			}
		}
	}

	return i;
}

void Playlist_BTN_newPL_onRelease(mgf::Element* e)
{
	if (g_app->m_playlistMgr)
		g_app->m_playlistMgr->AddNew();
}

int Playlist_LB_onIsItemSelected(mgf::ListBox* lb, void* item)
{
	PlayList* pl = (PlayList*)item;
	return (int)pl->m_isSelected;
}

void Playlist_LB_onItemClick(mgf::ListBox* lb, void* item, uint32_t itemIndex, uint32_t mouseButton)
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

int Playlist_LB_onDrawItem(mgf::ListBox* lb, void* item, uint32_t itemIndex, wchar_t** text, uint32_t* textlen)
{
	PlayList* pl = (PlayList*)item;

	*text = pl->m_name.data();
	*textlen = pl->m_name.size();

	return 1;
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
							node->m_audioFilePath = wstr.data();
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
	node->m_audioFilePath = fn;
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

const wchar_t* Playlist_onColTitleText(mgf::Table*, uint32_t* textLen, uint32_t colIndex)
{
	switch (colIndex)
	{
	case 0:
		*textLen = 7;
		return L"Playing";
	case 1:
		*textLen = 4;
		return L"File";
	case 2:
		*textLen = 1;
		return L"#";
	case 3:
		*textLen = 5;
		return L"Title";
	case 4:
		*textLen = 8;
		return L"Duration";
	}
	return 0;
}

int Playlist_onDrawRow(mgf::Table* tb, void* row, uint32_t col, wchar_t** text, uint32_t* textlen)
{
	if (row)
	{
		static mgf::StringW playingText = L"*";

		PlaylistNode* nd = (PlaylistNode*)row;
		static mgf::StringW textBuffer;
		// col is where you want to show text for current row
		switch (col)
		{
		case 0:
		{
			if (nd->m_isPlaying)
			{
				textBuffer.clear();
				textBuffer += playingText.data();
				*text = textBuffer.data();
				*textlen = textBuffer.size();
			}
			else
			{
				*text = 0;
				*textlen = 0;
			}
		}break;
		case 1:
			*text = nd->m_audioFilePath.data();
			*textlen = nd->m_audioFilePath.size();
			break;
		case 2:
			textBuffer.clear();
			textBuffer += nd->m_info_number.data();
			*text = textBuffer.data();
			*textlen = textBuffer.size();
			break;
		case 3:
			textBuffer.clear();
			textBuffer += nd->m_info_title.data();
			*text = textBuffer.data();
			*textlen = textBuffer.size();
			break;
		case 4:
			textBuffer.clear();
			textBuffer += nd->m_info_duration.data();
			*text = textBuffer.data();
			*textlen = textBuffer.size();
			break;
		}

		return 1;
	}
	return 0;
}

void Playlist_onRowClick(mgf::Table* tb, void* row, uint32_t rowIndex, uint32_t mouseButton, mgInputContext_s* input)
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
int Playlist_onIsRowSelected(mgf::Table* tb, void* row)
{
	PlaylistNode* nd = (PlaylistNode*)row;
	return (int)nd->m_isSelected;
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