#include "mgf.h"

#include "Log.h"

#include <cstdarg>
#include <string>

class Log
{
public:
	Log();
	~Log();

	enum E_MessageType
	{
		EMT_INFO,
		EMT_ERROR,
		EMT_WARNING
	};

	void print(E_MessageType mt) 
	{
		char buffer[1054];
		switch (mt)
		{
		case Log::EMT_INFO:
		{
			sprintf(buffer, "Info: %s", m_buffer);
			if (m_onInfo)
				m_onInfo(buffer);
			else
				fprintf(stdout, "%s", buffer);
		}break;
		case Log::EMT_ERROR:
		{
			sprintf(buffer, "Error: %s", m_buffer);
			if (m_onError)
				m_onError(buffer);
			else
				fprintf(stderr, "%s", buffer);
		}break;
		case Log::EMT_WARNING:
		{
			sprintf(buffer, "Warning: %s", m_buffer);
			if (m_onWarning)
				m_onWarning(buffer);
			else
				fprintf(stderr, "%s", buffer);
		}break;
		default:
			break;
		}
	}

	char m_buffer[1024];

	bool m_isInit = false;

	void(*m_onError)(const char* message) = nullptr;
	void(*m_onInfo)(const char* message) = nullptr;
	void(*m_onWarning)(const char* message) = nullptr;
};
Log::Log(){}
Log::~Log() {}

Log g_loger;

void mgf::LogWriteError(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	std::vsnprintf(g_loger.m_buffer, 1024, format, arg);
	va_end(arg);
	g_loger.print(Log::EMT_ERROR);
}

void mgf::LogWriteInfo(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	std::vsnprintf(g_loger.m_buffer, 1024, format, arg);
	va_end(arg);
	g_loger.print(Log::EMT_INFO);
}

void mgf::LogWriteWarning(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	std::vsnprintf(g_loger.m_buffer, 1024, format, arg);
	va_end(arg);
	g_loger.print(Log::EMT_WARNING);
}

void mgf::LogWriteErrorW(const wchar_t* format, ...)
{
	wchar_t buff[1024];
	std::memset(buff, 0, 1024 * sizeof(wchar_t));
	va_list arg;
	va_start(arg, format);
	std::vswprintf(buff, 1024, format, arg);
	va_end(arg);
	for (int i = 0; i < 1024; ++i)
	{
		g_loger.m_buffer[i] = (char)buff[i];
	}
	g_loger.print(Log::EMT_ERROR);
}

void mgf::LogWriteInfoW(const wchar_t* format, ...)
{
	wchar_t buff[1024];
	std::memset(buff, 0, 1024 * sizeof(wchar_t));
	va_list arg;
	va_start(arg, format);
	std::vswprintf(buff, 1024, format, arg);
	va_end(arg);
	for (int i = 0; i < 1024; ++i)
	{
		g_loger.m_buffer[i] = (char)buff[i];
	}
	g_loger.print(Log::EMT_INFO);
}

void mgf::LogWriteWarningW(const wchar_t* format, ...)
{
	wchar_t buff[1024];
	std::memset(buff, 0, 1024 * sizeof(wchar_t));
	va_list arg;
	va_start(arg, format);
	std::vswprintf(buff, 1024, format, arg);
	va_end(arg);
	for (int i = 0; i < 1024; ++i)
	{
		g_loger.m_buffer[i] = (char)buff[i];
	}
	g_loger.print(Log::EMT_WARNING);
}

void mgf::LogSetErrorOutput(void(*f)(const char* message))
{
	g_loger.m_onError = f;
}

void mgf::LogSetInfoOutput(void(*f)(const char* message))
{
	g_loger.m_onInfo = f;
}

void mgf::LogSetWarningOutput(void(*f)(const char* message))
{
	g_loger.m_onWarning = f;
}

