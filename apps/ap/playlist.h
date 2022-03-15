#pragma once
#ifndef AP_PL

//#include <string>
#include "framework/String.h"

#include <vector>

int Playlist_LB_onTextInputEndEdit(mgf::ListBox* lb, int i, const wchar_t* str, uint8_t* editItem);
wchar_t Playlist_LB_onTextInputCharEnter(mgf::ListBox* lb, wchar_t c);
void Playlist_BTN_newPL_onRelease(mgf::Element* e);

class PlayList;

struct PlaylistNode
{
	PlaylistNode() {}
	~PlaylistNode() {}

	mgf::StringW m_audioFilePath;
	uint32_t m_begin = 0;
	uint32_t m_end = 0;
};

struct PlaylistListBoxData
{
	PlaylistListBoxData() {}
	~PlaylistListBoxData() {}

	const wchar_t* text = 0;
	uint32_t flags = 0; /*0x1 - selected or not*/
	PlayList* playlist = 0;
};

class PlayList
{
public:
	PlayList();
	~PlayList();

	void RenameFile();
	void Save();

	mgf::StringW m_name;
	mgf::StringW m_filePath;

	std::vector<PlaylistNode*> m_nodes;
};


class PlayListManager : public mgf::BaseClass
{
	mgf::ListBox* m_listBox = 0;
	
	std::vector<PlayList*> m_playlists;
	std::vector<PlaylistListBoxData> m_playlistLBData;

	mgf::StringW m_musicDir;
	mgf::StringW m_playlistDir;

	PlayList* m_playPlaylist = 0;
	PlayList* m_editPlaylist = 0;

	void _updateGUIListbox();
public:
	PlayListManager(mgf::ListBox*);
	~PlayListManager();
	
	mgf::StringW CheckPLName(mgf::StringW*);

	void AddNew();
	
	void LoadPlaylist(const wchar_t*);
};

#endif // !AP_PL
