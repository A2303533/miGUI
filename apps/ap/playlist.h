#pragma once
#ifndef AP_PL

//#include <string>
#include "framework/String.h"

#include <vector>

int Playlist_LB_onTextInputEndEdit(mgf::ListBox* lb, int i, const wchar_t* str, uint8_t* editItem);
wchar_t Playlist_LB_onTextInputCharEnter(mgf::ListBox* lb, wchar_t c);
void Playlist_BTN_newPL_onRelease(mgf::Element* e);


struct PlaylistListBoxData
{
	PlaylistListBoxData() {}
	~PlaylistListBoxData() {}

	const wchar_t* text = 0;
	uint32_t flags = 0; /*0x1 - selected or not*/
};

class PlayList
{
public:
	PlayList();
	~PlayList();

	mgf::StringW m_name;
};


class PlayListManager : public mgf::BaseClass
{
	mgf::ListBox* m_listBox = 0;
	
	std::vector<PlayList*> m_playlists;
	std::vector<PlaylistListBoxData> m_playlistLBData;

	mgf::StringW m_musicDir;
	mgf::StringW m_playlistDir;

	mgf::StringW _checkPLName(mgf::StringW*);
public:
	PlayListManager(mgf::ListBox*);
	~PlayListManager();

	void AddNew();
};

#endif // !AP_PL
