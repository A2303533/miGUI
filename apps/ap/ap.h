#pragma once
#ifndef AP_H

struct IDropTarget;

class PlayListManager;

#define testTable_buttonsSize 50

// for callbacks
struct AP_global_data
{
	mgf::Framework* framework = 0;
	mgf::Context* context = 0;
	mgf::Window* mainWindow = 0;
	mgf::Rectangle* playlistArea = 0;
	mgf::Rectangle* controlArea = 0;
	mgf::Rectangle* tracklistArea = 0;
	mgf::Button* buttonNewPlaylist = 0;
	mgf::ListBox* listboxPlaylist = 0;
	mgf::Table* tableTracklist = 0;
	
	mgf::Button* tableTracklist_testButton[testTable_buttonsSize];
	int testTable_testButton_currBtn = 0;

	mgf::Font* popupFont = 0;

	mgStyle_s style;

	PlayListManager* playlistMgr = 0;
};

#endif // !AP_H
