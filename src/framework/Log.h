#ifndef _MGF_LOG_H_
#define _MGF_LOG_H_

namespace mgf
{

	void LogWriteError(const char* fmt, ...);
	void LogWriteInfo(const char* fmt, ...);
	void LogWriteWarning(const char* fmt, ...);
	void LogWriteErrorW(const wchar_t* fmt, ...);
	void LogWriteInfoW(const wchar_t* fmt, ...);
	void LogWriteWarningW(const wchar_t* fmt, ...);
	void LogSetErrorOutput(void(*)(const char* message));
	void LogSetInfoOutput(void(*)(const char* message));
	void LogSetWarningOutput(void(*)(const char* message));

	//#define MI_PRINT_FAILED	miLogWriteError( "Failed [%s][%s][%i]:(\n", MI_FILE, MI_FUNCTION, MI_LINE )

}

#endif 