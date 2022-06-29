/*
  Copyright (C) 2022 Basov Artyom
  The authors can be contacted at <artembasov@outlook.com>
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
	 notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
	 notice, this list of conditions and the following disclaimer in
	 the documentation and/or other materials provided with the
	 distribution.
  3. The names of the authors may not be used to endorse or promote
	 products derived from this software without specific prior
	 written permission.
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "mgf.h"
#include "FileSystem.h"

using namespace mgf;

class FileWin32 : public File
{
	TextFileInfo	m_textInfo;
	bool			m_isTextFile = false;
	HANDLE			m_handle = nullptr;

	// GENERIC_READ or GENERIC_WRITE
	uint64_t		m_pointerPosition = 0;
	DWORD			m_desiredAccess = 0;
public:
	FileWin32(const StringU16& fileName,
		FileMode mode,
		FileAccessMode access,
		FileAction action,
		FileShareMode EFSM,
		uint32_t EFA);
	virtual ~FileWin32();

	virtual TextFileInfo GetTextFileInfo() override;
	virtual void		SetTextFileInfo(TextFileInfo info) override;
	virtual uint32_t	Write(uint8_t* data, uint32_t size) override;
	virtual void		Write(const StringA& string) override;
	virtual void		Write(const StringW& string) override;
	virtual void	    Write(const StringU16& string) override;
	//virtual void		Write(const StringW32& string) override;
	virtual void		Flush() override;
	virtual uint64_t	Read(uint8_t* data, uint64_t size) override;
	virtual uint64_t	Size() override;
	virtual uint64_t	Tell() override;
	virtual void 		Seek(uint64_t distance, FileSeekPos pos) override;
};

FileWin32::FileWin32(const StringU16& fileName,
	FileMode mode,
	FileAccessMode access,
	FileAction action,
	FileShareMode EFSM,
	uint32_t EFA)
{
	if (mode == FileMode::Text)
	{
		m_isTextFile = true;
	}

	switch (access)
	{
	case FileAccessMode::Read:
		m_desiredAccess |= GENERIC_READ;
		break;
	case FileAccessMode::Write:
		m_desiredAccess |= GENERIC_WRITE;
		break;
	case FileAccessMode::Both:
		m_desiredAccess |= GENERIC_READ | GENERIC_WRITE;
		break;
	case FileAccessMode::Append:
		m_desiredAccess |= FILE_APPEND_DATA;
		break;
	}

	DWORD ShareMode = 0;
	switch (EFSM) {
	default:
	case FileShareMode::None:
		break;
	case FileShareMode::Delete:
		ShareMode = FILE_SHARE_DELETE;
		break;
	case FileShareMode::Read:
		ShareMode = FILE_SHARE_READ;
		break;
	case FileShareMode::Write:
		ShareMode = FILE_SHARE_WRITE;
		break;
	}

	DWORD CreationDisposition = 0;
	switch (action) {
	case FileAction::Open:
		CreationDisposition = OPEN_ALWAYS;
		break;
	case FileAction::Open_new:
		CreationDisposition = CREATE_ALWAYS;
		break;
	}

	DWORD FlagsAndAttributes = 0;
	if (EFA & (uint32_t)FileAttribute::Normal) {
		FlagsAndAttributes |= FILE_ATTRIBUTE_NORMAL;
	}
	else {
		if (EFA & (uint32_t)FileAttribute::Hidden) {
			FlagsAndAttributes |= FILE_ATTRIBUTE_HIDDEN;
		}
		if (EFA & (uint32_t)FileAttribute::Readonly) {
			FlagsAndAttributes |= FILE_ATTRIBUTE_READONLY;
		}
	}

	m_handle = CreateFileW((wchar_t*)fileName.data(), m_desiredAccess, ShareMode, NULL,
		CreationDisposition, FlagsAndAttributes, NULL);

	if (!m_handle)
	{
		//printWarning( u"Can not create file [%s], error code[%u]",fileName.data(), GetLastError() );
	}
}


FileWin32::~FileWin32()
{
	if (m_handle)
	{
		CloseHandle(m_handle);
		m_handle = nullptr;
	}
}


uint32_t	FileWin32::Write(uint8_t* data, uint32_t size)
{

	assert(m_desiredAccess & GENERIC_WRITE);

	if (!m_handle)
	{
		//printWarning( u"Can not write text to file. m_handle == nullptr" );
		return 0;
	}

	DWORD bytesWritten = 0;
	if (WriteFile(m_handle, data, size, &bytesWritten, NULL) == FALSE)
	{
		//printWarning( u"Can not write text to file. Error code [%u]", GetLastError() );
	}
	else
		m_pointerPosition += size;

	return bytesWritten;
}

void	FileWin32::Write(const StringA& string)
{
	assert(m_isTextFile);
	if (!m_handle)
	{
		fprintf(stderr, "Can not write text to file. m_handle == nullptr\n");
		return;
	}

	DWORD bytesWritten;
	if (WriteFile(m_handle, string.c_str(), (DWORD)string.size(), &bytesWritten, NULL) == FALSE)
	{
		fprintf(stderr, "Can not write text to file. Error code [%u]\n", GetLastError());
	}
	else
		m_pointerPosition += string.size();
}

void	FileWin32::Write(const StringW& string)
{
	assert(m_isTextFile);
	if (!m_handle)
	{
		fprintf(stderr, "Can not write text to file. m_handle == nullptr\n");
		return;
	}

	DWORD bytesWritten;
	if (WriteFile(m_handle, string.c_str(), (DWORD)string.size() * sizeof(wchar_t), &bytesWritten, NULL) == FALSE)
	{
		fprintf(stderr, "Can not write text to file. Error code [%u]\n", GetLastError());
	}
	else
		m_pointerPosition += string.size() * sizeof(wchar_t);
}

void FileWin32::Write(const StringU16& string)
{
	assert(m_isTextFile);
	if (!m_handle)
	{
		fprintf(stderr, "Can not write text to file. m_handle == nullptr\n");
		return;
	}

	DWORD bytesWritten;
	if (WriteFile(m_handle, string.c_str(), (DWORD)string.size() * sizeof(char16_t), &bytesWritten, NULL) == FALSE)
	{
		fprintf(stderr, "Can not write text to file. Error code [%u]\n", GetLastError());
	}
	else
		m_pointerPosition += string.size() * sizeof(char16_t);
}

//void	FileWin32::write(const StringW32& string)
//{
//	assert(m_isTextFile);
//	if (!m_handle)
//	{
//		fprintf(stderr, "Can not write text to file. m_handle == nullptr\n");
//		return;
//	}
//
//	DWORD bytesWritten;
//	if (WriteFile(m_handle, string.c_str(), (DWORD)string.size() * sizeof(char32_t), &bytesWritten, NULL) == FALSE)
//	{
//		fprintf(stderr, "Can not write text to file. Error code [%u]\n", GetLastError());
//	}
//	else
//		m_pointerPosition += string.size() * sizeof(char32_t);
//}

void	FileWin32::Flush()
{
	if (m_handle)
	{
		FlushFileBuffers(m_handle);
	}
}


uint64_t	FileWin32::Read(uint8_t* data, uint64_t size)
{
	assert(m_desiredAccess & GENERIC_READ);
	if (m_handle)
	{
		DWORD readBytesNum = 0;
		if (ReadFile(m_handle,
			data, (DWORD)size,
			&readBytesNum,
			NULL
		) == FALSE)
		{
			//printWarning( u"Can not read file. Error code [%u]", GetLastError() );
		}
		return readBytesNum;
	}
	return 0;
}

uint64_t		FileWin32::Size()
{
	if (!m_handle)
	{
		//printWarning( u"Can not get file size. m_handle == nullptr" );
		return 0;
	}
	return static_cast<uint64_t>(GetFileSize(m_handle, NULL));
}


uint64_t		FileWin32::Tell()
{
	return m_pointerPosition;
}


void		FileWin32::Seek(uint64_t distance, FileSeekPos pos)
{
	if ((m_desiredAccess & GENERIC_READ) || (m_desiredAccess & GENERIC_WRITE))
	{
		if (m_handle)
		{

			LARGE_INTEGER li;
			li.QuadPart = distance;

			li.LowPart = SetFilePointer(m_handle, li.LowPart, (PLONG)&li.HighPart, (DWORD)pos);
			m_pointerPosition = li.QuadPart;

		}
	}
}

TextFileInfo	FileWin32::GetTextFileInfo()
{
	return m_textInfo;
}

void			FileWin32::SetTextFileInfo(TextFileInfo info)
{
	m_textInfo = info;
}

// ===========================================================================

FileSystem::FileSystem()
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	m_exePath.assign((char16_t*)szFileName);

	m_exePath.flip_slash();
	//util::stringFlipSlash<StringW>(m_exePath);

	m_exePath.pop_back_before(L'/');
	//util::stringPopBackBefore<StringW>(m_exePath, '/');

	GetSystemDirectory(szFileName, MAX_PATH);
	m_systemPath.assign((char16_t*)szFileName);
	m_systemPath += u"/";
	//util::stringFlipSlash<StringW>(m_systemPath);
	m_systemPath.flip_slash();
}

FileSystem::~FileSystem()
{
	if (hFind)
		CloseHandle(hFind);
}

File* FileSystem::CreateFile(const StringU16& fileName,
	FileMode mode,
	FileAccessMode access,
	FileAction action,
	FileShareMode EFSM,
	uint32_t EFA
)
{
	//return new kkFileWin32( fileName, access, action, EFSM, EFA );
	return new FileWin32(fileName, mode, access, action, EFSM, EFA);
}

bool FileSystem::DeleteFile(const StringW& fileName)
{
	if (DeleteFileW((wchar_t*)fileName.data()) == FALSE)
	{
		//printWarning( u"Can not delete file [%s]. Error code [%u]", fileName.data(), GetLastError() );
		return false;
	}
	
	return true;
}

bool FileSystem::ExistFile(const StringW& fileName)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile((wchar_t*)fileName.data(), &FindFileData);
	int found = handle != INVALID_HANDLE_VALUE;
	if (found)
	{
		FindClose(handle);
		return true;
	}
	return false;
}

bool FileSystem::ExistFile(const StringA& file)
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE handle = FindFirstFileA(file.data(), &FindFileData);
	int found = handle != INVALID_HANDLE_VALUE;
	if (found)
	{
		FindClose(handle);
		return true;
	}
	return false;
}

bool FileSystem::ExistFile(const StringU16& file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile((wchar_t*)file.data(), &FindFileData);
	int found = handle != INVALID_HANDLE_VALUE;
	if (found)
	{
		FindClose(handle);
		return true;
	}
	return false;
}

bool FileSystem::ExistDir(const StringW& dir)
{
	DWORD dwAttrib = GetFileAttributes((wchar_t*)dir.data());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void FileSystem::DeleteFolder(const StringW& dir)
{
	std::vector<FileSystem::DirObject> objs;
	FileSystem::ScanDirBegin(dir);
	FileSystem::DirObject ob;
	while (FileSystem::GetDirObject(&ob))
	{
		objs.push_back(ob);
	}
	FileSystem::ScanDirEnd();
	uint32_t sz = (uint32_t)objs.size();

	for (uint32_t i = 0; i < sz; ++i)
	{
		auto* o = &objs[i];
		if (o->type == FileSystem::DirObjectType::info)
		{
			continue;
		}
		else if (o->type == FileSystem::DirObjectType::folder)
		{
			DeleteFolder((char16_t*)o->path);
		}
		else if (o->type == FileSystem::DirObjectType::file)
		{
			StringW path((char16_t*)o->path);
			if (DeleteFile(o->path) == FALSE)
			{
			//	fprintf(stderr, "Can not delete file [%s]. Error code [%u]\n", StringW(o->path).to_StringWA().data(), GetLastError());
			}
		}
	}

	if (RemoveDirectory((wchar_t*)dir.data()) == FALSE)
	{
	//	fprintf(stderr, "Can not remove directory [%s]. Error code [%u]\n", dir.to_StringWA().data(), GetLastError());
	}
}

bool FileSystem::DeleteDir(const StringW& dir)
{
	DeleteFolder(dir);
	return true;
}

bool FileSystem::CreateDir(const StringW& dir)
{
	if (CreateDirectory((wchar_t*)dir.data(), NULL) == FALSE)
	{
		/*DWORD error = GetLastError();
		fprintf(stderr, "Can not create directory [%s]. Error code [%u]\n", dir.to_StringWA().c_str(), error);
		if (error == ERROR_ALREADY_EXISTS)
			fprintf(stderr, "Directory already exists.\n");
		else if (error == ERROR_PATH_NOT_FOUND)
			fprintf(stderr, "One or more intermediate directories do not exist.\n");*/
		return false;
	}
	return true;
}

void FileSystem::ScanDirBegin(StringW dir)
{
	if (!m_dirScanBegin) {
		m_dirScanBegin = true;

		m_dir.clear();

		m_dir = dir;

		if (m_dir[m_dir.size() - 1] != u'/' && m_dir[m_dir.size() - 1] != u'\\')
			m_dir.append(u"\\*");
		else m_dir.append(u"*");

	}
}

void FileSystem::ScanDirEnd()
{
	if (m_dirScanBegin)
	{
		m_dirScanBegin = false;
		m_firstCall = false;

		if (hFind)
			FindClose(hFind);
		hFind = nullptr;
	}
}


bool FileSystem::GetDirObject(FileSystem::DirObject* o)
{
	WIN32_FIND_DATA ffd;

	if (!m_firstCall) {
		m_firstCall = true;
		if (hFind)
			FindClose(hFind);

		hFind = FindFirstFile((wchar_t*)m_dir.data(), &ffd);
		if (INVALID_HANDLE_VALUE == hFind) {
			DWORD error = GetLastError();
			fprintf(stderr, "Can not scan dir. Error code [%u].\n", error);
			if (error == ERROR_FILE_NOT_FOUND) {
				fprintf(stderr, "No matching files can be found.\n");
			}
			m_dirScanBegin = false;
			return false;
		}
	}
	else
	{
		if (FindNextFile(hFind, &ffd) == FALSE)
		{
			return false;
		}
	}

	if (ffd.cFileName[0] == L'.')
	{
		o->size = 0;
		o->type = FileSystem::DirObjectType::info;
	}
	else {
		StringW fullPath(m_dir);
		fullPath.pop_back();// '*'
		fullPath += (char16_t*)ffd.cFileName;
#ifdef _MSC_VER
		wcscpy_s(o->path, MGF_MAX_PATH, (wchar_t*)fullPath.data());
#else
		wcscpy(o->path, (wchar_t*)fullPath.data());
#endif

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			o->type = FileSystem::DirObjectType::folder;
			o->size = 0;
		}
		else
		{
			o->type = FileSystem::DirObjectType::file;

			LARGE_INTEGER i;
			i.LowPart = ffd.nFileSizeLow;
			i.HighPart = static_cast<LONG>(ffd.nFileSizeHigh);

			o->size = static_cast<uint32_t>(i.QuadPart);
		}
	}

	return true;
}

bool FileSystem::CopyFile(const StringW& existingFileName, const StringW& newFileName, bool overwrite)
{
	if (!this->ExistFile(existingFileName))
	{
		//fprintf(stderr, "Can not copy file [%s]. File not exist.\n", existingFileName.to_StringWA().data());
		return false;
	}

	DWORD flag = COPY_FILE_FAIL_IF_EXISTS;
	if (overwrite) flag = 0;

	if (CopyFileEx((LPWCH)existingFileName.data(),
		(LPWCH)newFileName.data(),
		NULL, NULL, NULL, flag)
		== FALSE)
	{
	//	fprintf(stderr, "Can not copy file [%s]. Error code [%u].\n", existingFileName.to_StringWA().data(), GetLastError());
		return false;
	}

	return true;
}

StringW FileSystem::GetProgramPath()
{
	return m_exePath;
}


StringW FileSystem::GetSystemPath()
{
	return m_systemPath;
}

StringW FileSystem::GetRealPath(const StringW& in)
{
	uint32_t dots_count = 0;

	size_t in_sz = in.size();

	size_t last = in_sz - 1;

	for (size_t i = 0; i < in_sz; ++i)
	{
		if (in[i] == u'.')
		{
			if (i < last)
			{
				if (in[i + 1] == u'.')
				{
					++dots_count;
					++i;// skip second .
				}
			}
		}
	}

	StringW realPath = m_exePath;
	StringW relativePath = in;
	for (uint32_t i = 0; i < dots_count; ++i)
	{
		//util::stringPopBackBefore(realPath, '/');
		realPath.pop_back_before(L'/');
		relativePath.erase(0, 2);
	}

	realPath += relativePath;


	return realPath;
}

