#include "CRT.h"
#include "stdio.h"

#ifdef __CRT_WIN32
#include <Windows.h>
#endif

//MyLib g_lib;
//
//#include <Windows.h>
//
//MyLib::MyLib()
//{
//	AttachConsole(-1);
//	stdout = CreateFileA("CONOUT$", (0x80000000L) | (0x40000000L), 0x00000001 | 0x00000002, 0, 3, 0x00000080, 0);
//}
//
//MyLib::~MyLib()
//{
//	if (stdout)
//		CloseHandle(stdout);
//}
//
//
//void mgf::printf(const char* str)
//{
//	int num = 0;	const char* ptr = str;	while (*ptr++)	{		++num;	}
//	
////	SetStdHandle(((int)-11), 1);
//	//AllocConsole();
//	//g_rtl.stdout = CreateFileA("CONOUT$", (0x80000000L) | (0x40000000L), 0x00000001 | 0x00000002, 0, 3, 0x00000080, 0);
//	//WriteConsoleA(stdout, str, num, &nw, 0);
//	WriteConsoleA(g_lib.stdout, str, num, 0, 0);
//}
