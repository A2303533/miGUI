#pragma once
#ifndef AP_H

struct IDropTarget;

class PlayListManager;

#define testTable_buttonsSize 50

enum class backend_type
{
	Gdi,
	OpenGLOld,
};

class ButtonNewPlaylist;
class ListboxPlaylist;

class AP_application
{
public:
	AP_application() {}
	~AP_application();

	bool Init(backend_type);
	void Run();

	mgf::Framework* m_framework = 0;
	mgf::SystemWindow* m_sysWindow = 0;
	mgf::AudioEngine* m_audio = 0;
	mgf::Backend* m_backend = 0;

	mgf::Context* m_context = 0;
	mgf::Window* m_mainWindow = 0;
	mgf::Rectangle* m_playlistArea = 0;
	mgf::Rectangle* m_controlArea = 0;
	mgf::Rectangle* m_tracklistArea = 0;
	ButtonNewPlaylist* m_buttonNewPlaylist = 0;
	ListboxPlaylist* m_listboxPlaylist = 0;
	mgf::Table* m_tableTracklist = 0;
	
	mgf::Button* m_tableTracklist_testButton[testTable_buttonsSize];
	int m_testTable_testButton_currBtn = 0;

	mgf::Font* m_popupFont = 0;
	mgf::Font* m_fontImpact = 0;
	mgf::Font* m_listboxFont = 0;

	mgf::Window* m_guiWindow = 0;

	mgf::Icons* m_icons = 0;

	mgStyle_s m_style;

	PlayListManager* m_playlistMgr = 0;
};

#endif // !AP_H
