#include "framework/mgf.h"
#include "framework/ListBox.h"
#include "framework/TextBuffer.h"
#include "ap.h"
#include "playlist.h"

#include <filesystem>
#include <fstream>

#include <ShlObj.h>

extern AP_global_data g_data;

int Playlist_LB_onTextInputEndEdit(mgf::ListBox* lb, int i, const wchar_t* str, void* editItem);
wchar_t Playlist_LB_onTextInputCharEnter(mgf::ListBox* lb, wchar_t c);
int Playlist_LB_onIsItemSelected(mgf::ListBox* lb, void* item);
void Playlist_LB_onItemClick(mgf::ListBox* lb, void* item, uint32_t itemIndex, uint32_t mouseButton);
int Playlist_LB_onDrawItem(mgf::ListBox*, void* item, uint32_t itemIndex, wchar_t** text, uint32_t* textlen);
void Playlist_BTN_newPL_onRelease(mgf::Element* e);

PlayList::PlayList()
{

}

PlayList::~PlayList()
{
	for (uint32_t i = 0, sz = m_nodes.size(); i < sz; ++i)
	{
		delete m_nodes[i];
	}
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
			fwrite(L"s:", 2 * sizeof(wchar_t), 1, f);
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

	newFilePath = g_data.playlistMgr->CheckPLName(&newFilePath);
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

PlayListManager::PlayListManager(mgf::ListBox* lb)
	:
	m_listBox(lb)
{
	m_listBox->onIsItemSelected = Playlist_LB_onIsItemSelected;
	m_listBox->onItemClick = Playlist_LB_onItemClick;
	m_listBox->onDrawItem = Playlist_LB_onDrawItem;

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

	m_stateFilePath = g_data.framework->GetAppDir()->data();
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

	mgf::StringA stra = plFilePath.to_utf8();
	FILE* f = fopen(stra.data(), "wb");
	if (f)
	{
		uint16_t bom = 0xFEFF;
		fwrite(&bom, 2, 1, f);
		fclose(f);
	}

	newPL->m_filePath = plFilePath.data();
	newPL->m_fileName = std::filesystem::path(plFilePath.data()).filename().c_str();
	m_playlists.emplace_back(newPL);

	_updateGUIListbox();
	
	newPL->Save();
}

void PlayListManager::_updateGUIListbox()
{
	g_data.listboxPlaylist->SetData((void**)m_playlists.data(), m_playlists.size());
}

void PlayListManager::SaveStateFile()
{
	mgf::StringA stra = m_stateFilePath.to_utf8();
	FILE* f = fopen(stra.data(), "wb");
	if (f)
	{
		uint16_t bom = 0xFEFF;
		fwrite(&bom, 2, 1, f);
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

int Playlist_LB_onTextInputEndEdit(mgf::ListBox* lb, int i, const wchar_t* str, void* editItem)
{
	/*
	* i:
	*	1 - Enter
	*   2 - click somewhere
	*   3 - Escape
	*/

	if (str)
	{
		/*wprintf(L"END: %s\n", str);*/
		if (i == 1)
		{
			uint32_t len = wcslen(str);
			//if (len < 30)
			//{
			//	PlaylistListBoxData* data = (PlaylistListBoxData*)editItem;
			//	/* data->text points to not const wchar_t
			//	*   so we can change text.
			//	*/
			//	swprintf((wchar_t*)data->text, L"%s", str);
			//}
			if (len)
			{
				//PlaylistListBoxData* data = (PlaylistListBoxData*)editItem;
				//data->playlist->m_name = str;
				//data->text = data->playlist->m_name.data();
				//data->playlist->RenameFile();
			}
		}
	}

	return i;
}

void Playlist_BTN_newPL_onRelease(mgf::Element* e)
{
	if (g_data.playlistMgr)
		g_data.playlistMgr->AddNew();
}

int Playlist_LB_onIsItemSelected(mgf::ListBox* lb, void* item)
{
	PlayList* pl = (PlayList*)item;
	return (int)pl->m_isSelected;
}

void Playlist_LB_onItemClick(mgf::ListBox* lb, void* item, uint32_t itemIndex, uint32_t mouseButton)
{
	if (mouseButton)
	{
		PlayList* pl = (PlayList*)item;
		pl->m_mgr->SelectPlaylist(pl);
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
	for (size_t i = 0, sz = m_playlists.size(); i < sz; ++i)
	{
		m_playlists[i]->m_isSelected = false;
	}
	pl->m_isSelected = true;
	m_editPlaylist = pl;
	SaveStateFile();
}
