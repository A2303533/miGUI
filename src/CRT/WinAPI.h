#pragma once
#ifndef __WINAPI__
#define __WINAPI__

#ifdef __cplusplus
extern "C" {
#endif

	typedef unsigned long DWORD, * PDWORD, * LPDWORD;

	__declspec(dllimport) int __stdcall MessageBoxA( void*, const char*, const char*, unsigned int);
	__declspec(dllimport) void* __stdcall GetProcessHeap(void);
	#define HEAP_ZERO_MEMORY                0x00000008      
	__declspec(dllimport) __declspec(allocator) void* __stdcall HeapAlloc(void*, unsigned int, size_t);
	__declspec(dllimport) int __stdcall HeapFree(void*, unsigned int, void*);
	__declspec(dllimport) __declspec(allocator) void* __stdcall HeapReAlloc(void*,unsigned int, void*,size_t);
	__declspec(dllimport) void* __stdcall GetCurrentProcess(void);
	__declspec(dllimport) int __stdcall TerminateProcess(void*,unsigned int);
	__declspec(dllimport) int __stdcall DeleteFileA( const char* lpFileName );
	__declspec(dllimport) int __stdcall MoveFileA(const char* lpExistingFileName, const char* lpNewFileName);
	__declspec(dllimport) unsigned int __stdcall GetFileAttributesA(const char* lpFileName);
	__declspec(dllimport) unsigned int __stdcall GetFileAttributesW(const wchar_t* lpFileName);
	__declspec(dllimport) void* __stdcall CreateFileA(const char* lpFileName,unsigned int dwDesiredAccess,
		unsigned int dwShareMode, void* lpSecurityAttributes, unsigned int dwCreationDisposition,
		unsigned int dwFlagsAndAttributes, void* hTemplateFile);
	__declspec(dllimport) int __stdcall CloseHandle(void* hObject);
	__declspec(dllimport) int __stdcall FlushFileBuffers(void* hFile);
	__declspec(dllimport) int __stdcall AllocConsole(void);
	__declspec(dllimport) int __stdcall AttachConsole(unsigned int dwProcessId);
	__declspec(dllimport) void* __stdcall GetStdHandle(unsigned int nStdHandle);
	__declspec(dllimport) int __stdcall WriteFile(void* hFile,const char* lpBuffer,
		unsigned int nNumberOfBytesToWrite,unsigned int* lpNumberOfBytesWritten,
		void* lpOverlapped);
	__declspec(dllimport) int __stdcall SetStdHandle(unsigned int  nStdHandle, void* hHandle);

#define INVALID_FILE_SIZE ((unsigned int)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((unsigned int)-1)
#define INVALID_FILE_ATTRIBUTES ((unsigned int)-1)
#define FILE_SHARE_READ                 0x00000001  
#define FILE_SHARE_WRITE                0x00000002  
#define FILE_SHARE_DELETE               0x00000004  
#define FILE_ATTRIBUTE_READONLY             0x00000001  
#define FILE_ATTRIBUTE_HIDDEN               0x00000002  
#define FILE_ATTRIBUTE_SYSTEM               0x00000004  
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010  
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020  
#define FILE_ATTRIBUTE_DEVICE               0x00000040  
#define FILE_ATTRIBUTE_NORMAL               0x00000080  
#define FILE_ATTRIBUTE_TEMPORARY            0x00000100  
#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200  
#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400  
#define FILE_ATTRIBUTE_COMPRESSED           0x00000800  
#define FILE_ATTRIBUTE_OFFLINE              0x00001000  
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000  
#define FILE_ATTRIBUTE_ENCRYPTED            0x00004000  
#define FILE_ATTRIBUTE_INTEGRITY_STREAM     0x00008000  
#define FILE_ATTRIBUTE_VIRTUAL              0x00010000  
#define FILE_ATTRIBUTE_NO_SCRUB_DATA        0x00020000  
#define FILE_ATTRIBUTE_EA                   0x00040000  
#define FILE_ATTRIBUTE_PINNED               0x00080000  
#define FILE_ATTRIBUTE_UNPINNED             0x00100000  
#define FILE_ATTRIBUTE_RECALL_ON_OPEN       0x00040000  
#define FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS 0x00400000 
#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)
#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5


#ifdef __cplusplus
}
#endif

#endif