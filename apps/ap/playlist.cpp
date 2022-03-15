#include "framework/mgf.h"
#include "framework/ListBox.h"
#include "ap.h"
#include "playlist.h"

#include <filesystem>
#include <fstream>

#include <ShlObj.h>

extern AP_global_data g_data;

int Playlist_LB_onTextInputEndEdit(mgf::ListBox* lb, int i, const wchar_t* str, uint8_t* editItem);
wchar_t Playlist_LB_onTextInputCharEnter(mgf::ListBox* lb, wchar_t c);
int Playlist_LB_onSelect(mgf::ListBox* lb, uint8_t* item);
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
	Save();
}

PlayListManager::PlayListManager(mgf::ListBox* lb)
	:
	m_listBox(lb)
{
	m_listBox->onSelect = Playlist_LB_onSelect;

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

	m_playlistDir = m_musicDir;
	m_playlistDir += "/AP/";
	if (!std::filesystem::exists(m_playlistDir.data()))
		std::filesystem::create_directory(m_playlistDir.data());

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
	PlayList* newPL = new PlayList;
	newPL->m_filePath = pth;

	std::filesystem::path p = pth;
	std::filesystem::path fn = p.filename();

	newPL->m_name = fn.c_str();
	newPL->m_name.pop_back_before(L'.');
	newPL->m_name.pop_back();

	m_playlists.emplace_back(newPL);
	m_playlistLBData.emplace_back(PlaylistListBoxData());

	_updateGUIListbox();
}

void PlayListManager::AddNew()
{
	static uint32_t pl_number = 0;

	PlayList* newPL = new PlayList;
	
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
	m_playlists.emplace_back(newPL);	
	m_playlistLBData.emplace_back(PlaylistListBoxData());

	_updateGUIListbox();

	newPL->Save();
}

void PlayListManager::_updateGUIListbox()
{
	for (uint32_t i = 0; i < m_playlists.size(); ++i)
	{
		m_playlistLBData[i].text = m_playlists[i]->m_name.data();
		m_playlistLBData[i].playlist = m_playlists[i];
	}
	g_data.listboxPlaylist->SetData(m_playlistLBData.data(), m_playlistLBData.size());
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

int Playlist_LB_onTextInputEndEdit(mgf::ListBox* lb, int i, const wchar_t* str, uint8_t* editItem)
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
				PlaylistListBoxData* data = (PlaylistListBoxData*)editItem;
				data->playlist->m_name = str;
				data->text = data->playlist->m_name.data();
				data->playlist->RenameFile();
				/*std::filesystem::rename*/
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

int Playlist_LB_onSelect(mgf::ListBox* lb, uint8_t* item)
{
	PlaylistListBoxData* data = (PlaylistListBoxData*)item;
	wprintf(L"%s\n", data->text);
	return 1;
}

