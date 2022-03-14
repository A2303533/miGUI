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
void Playlist_BTN_newPL_onRelease(mgf::Element* e);

PlayList::PlayList()
{

}

PlayList::~PlayList()
{

}

PlayListManager::PlayListManager(mgf::ListBox* lb)
	:
	m_listBox(lb)
{
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


}

PlayListManager::~PlayListManager()
{
	for (uint32_t i = 0; i < m_playlists.size(); ++i)
	{
		delete m_playlists[i];
	}
}

mgf::StringW PlayListManager::_checkPLName(mgf::StringW* str)
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
		s.pop_back();

		if (i)
			s += i;

		s += ".appl";

		if (!std::filesystem::exists(s.data()))
			break;
	}

	return s;
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

	plFilePath = _checkPLName(&plFilePath);
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
		fwrite(newPL->m_name.data(), newPL->m_name.size() * sizeof(wchar_t), 1, f);
		wchar_t nl = '\n';
		fwrite(&nl, 2, 1, f);
		fclose(f);
	}
	//std::ofstream out(plFilePath.data());
	//out.write():

	m_playlists.emplace_back(newPL);
	m_playlistLBData.emplace_back(PlaylistListBoxData());

	for (uint32_t i = 0; i < m_playlists.size(); ++i)
	{
		m_playlistLBData[i].text = m_playlists[i]->m_name.data();
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
		if (i == 1 || i == 2)
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
