#include "miGUI.h"

class Demo
{
public:
	Demo();
	~Demo();

	void Init(migui::VideoDriverAPI*, migui::InputContext*);

	migui::GUIContext* m_gui = 0;
};

Demo::Demo()
{
	m_gui = new migui::GUIContext;
}

Demo::~Demo()
{
	if (m_gui) delete m_gui;
}

void Demo::Init(migui::VideoDriverAPI* gpu, migui::InputContext* input)
{
	m_gui->m_gpu = gpu;
	m_gui->m_input = input;
}