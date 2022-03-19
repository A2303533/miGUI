#pragma once
#ifndef AP_H

class PlayListManager;

// for callbacks
struct AP_global_data
{
	mgf::Window* mainWindow = 0;
	mgf::Rectangle* playlistArea = 0;
	mgf::Rectangle* controlArea = 0;
	mgf::Rectangle* tracklistArea = 0;
	mgf::Button* buttonNewPlaylist = 0;
	mgf::ListBox* listboxPlaylist = 0;
	mgf::Table* tableTracklist = 0;
	mgf::Button* tableTracklist_testButton = 0;
	mgStyle_s style;

	PlayListManager* playlistMgr = 0;
};

#endif // !AP_H
