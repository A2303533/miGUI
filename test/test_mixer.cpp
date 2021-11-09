#include "mixer.lib.h"
#include "mixer.lib.window.h"
#include "mixer.lib.videoDriver.h"
#include "mixer.lib.event.h"
#include "mixer.lib.inputContext.h"

#include <filesystem>

void window_onCLose(miWindow* window) {
	miQuit();
}

void log_onError(const char* message) {
	fprintf(stderr, message);
}

void log_onInfo(const char* message) {
	fprintf(stdout, message);
}

int main(int argc, char* argv[])
{
	const char* videoDriverType = "mixer.vd.opengl33.dll"; // for example read name from .ini
	miStringA videoDriverTypeStr = videoDriverType;
	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "-vid") == 0)
		{
			++i;
			if (i < argc)
			{
				videoDriverTypeStr = argv[i];
			}
		}
	}

	miInputContext* m_inputContext = miCreate<miInputContext>();
	miLibContext* m_libContext = miCreate<miLibContext>();
	m_libContext->Start(m_inputContext);

	// set callbacks if you want to read some information 
	miLogSetErrorOutput(log_onError);
	miLogSetInfoOutput(log_onInfo);
	miLogSetWarningOutput(log_onError);

	u32 windowStyle = 0;
	miWindow* m_window = miCreateWindow(800, 600, windowStyle, 0);
	m_window->m_onClose = window_onCLose;

	if (!miInitVideoDriver(videoDriverTypeStr.data(), m_window))
	{
		miLogWriteWarning("Can't load video driver : %s\n", videoDriverTypeStr.data());
		for (auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
		{
			auto path = entry.path();
			if (path.has_extension())
			{
				auto ex = path.extension();
				if (ex == ".dll")
				{
					miLogWriteWarning("Trying to load video driver : %s\n", path.generic_string().c_str());

					if (miInitVideoDriver(path.generic_string().c_str(), m_window))
					{
						goto vidOk;
					}
					else
					{
						miLogWriteWarning("Can't load video driver : %s\n", path.generic_string().c_str());
					}
				}
			}
		}
		MI_PRINT_FAILED;
		return false;
	}

vidOk:

	miVideoDriver* m_gpu = miGetVideoDriver();
	m_gpu->SetClearColor(0.3f, 0.3f, 0.74f, 1.f);
	m_window->SetTitle(m_gpu->GetVideoDriverName());

	f32 m_dt = 0.f;
	miEvent currentEvent;

	while (miRun(&m_dt))
	{
		while (miPollEvent(currentEvent))
		{
			switch (currentEvent.m_type)
			{
			default:
			case miEventType::Engine:
				break;
			case miEventType::System:
				break;
			case miEventType::Window: {
				if (currentEvent.m_event_window.m_event == miEvent_Window::size_changed) {
					m_gpu->UpdateMainRenderTarget(v2f((f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y));
					//m_guiContext->NeedRebuild();
				}
			}break;
			}
		}

		switch (*m_libContext->m_state)
		{
		default:
			break;
		case miSystemState::Run:
		{
			m_gpu->BeginDraw();
			m_gpu->ClearAll();
			m_gpu->UseDepth(true);

			m_gpu->DrawRectangle(v4f(0.f, 0.f, 40.f, 15.f), ColorRed, ColorBlue);

			m_gpu->BeginDrawGUI();
			//m_guiContext->DrawAll();

			m_gpu->EndDrawGUI();

			m_gpu->EndDraw();
			m_gpu->SwapBuffers();

		}break;
		}
	}

	if (m_window) miDestroy(m_window);
	if (m_libContext) miDestroy(m_libContext);
	if (m_inputContext) miDestroy(m_inputContext);

	return 1;
}