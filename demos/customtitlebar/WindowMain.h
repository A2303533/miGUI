#pragma once
#ifndef __WINDOWMAIN_H_
#define __WINDOWMAIN_H_

#define MainMenuPositionXWindowed 30
#define MainMenuPositionX 35
#define MainMenuPositionY 9
#define TitlebarRightPartSize 300

class WindowMain : public mgf::SystemWindow
{
	bool m_isMaximized = false;
	uint32_t m_cursorInSystemButtons = 0;
	uint32_t m_pushedSystemButton = 0;
	
	Application* m_app = 0;
public:
	WindowMain(Application*, int windowFlags, const mgPoint& windowPosition, const mgPoint& windowSize);
	virtual ~WindowMain();

	virtual void OnDrawCustomTitleBar() override;
	virtual void OnMaximize() override;
	virtual void OnRestore() override;
	virtual void OnMove() override;
	virtual void OnSize() override;
	virtual bool OnHTCaption() override;
};

#endif
