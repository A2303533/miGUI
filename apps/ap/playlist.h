#pragma once
#ifndef AP_PL

//#include <string>
#include "framework/String.h"

#include <vector>

int Playlist_LB_onTextInputEndEdit(mgf::ListBox* lb, int i, const wchar_t* str, void* editItem);
wchar_t Playlist_LB_onTextInputCharEnter(mgf::ListBox* lb, wchar_t c);
void Playlist_BTN_newPL_onRelease(mgf::Element* e);

class PlayList;
class PlayListManager;

struct PlaylistNode
{
	PlaylistNode() {}
	~PlaylistNode() {}

	mgf::StringW m_audioFilePath;
	uint32_t m_begin = 0;
	uint32_t m_end = 0;
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
	mgf::StringW m_fileName;

	std::vector<PlaylistNode*> m_nodes;
	PlayListManager* m_mgr = 0;

	bool m_isSelected = false;
};


class PlayListManager : public mgf::BaseClass
{
	mgf::ListBox* m_listBox = 0;
	
	std::vector<PlayList*> m_playlists;

	mgf::StringW m_musicDir;
	mgf::StringW m_playlistDir; 

	mgf::StringW m_stateFilePath;

	PlayList* m_playPlaylist = 0; // active for playing
	PlayList* m_editPlaylist = 0; // active for table

	void _updateGUIListbox();
public:
	PlayListManager(mgf::ListBox*);
	virtual ~PlayListManager();
	
	mgf::StringW CheckPLName(mgf::StringW*);

	void AddNew();
	
	void LoadPlaylist(const wchar_t*);
	void SaveStateFile();

	void SetEditPlaylist(PlayList*);
	void SelectPlaylist(PlayList*);
};

#endif // !AP_PL
