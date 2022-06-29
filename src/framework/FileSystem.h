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

#pragma once
#ifndef _MGF_FS_H_
#define _MGF_FS_H_

#include "String.h"
#include "OS/UndefWindows.h"
#include "XML.h"

namespace mgf
{
	constexpr uint32_t MGF_MAX_PATH = 0xffff;

	enum class TextFileFormat : uint32_t
	{
		UTF_8,
		UTF_16,
		UTF_32
	};

	enum class TextFileEndian : uint32_t
	{
		Little, //	0xFFFE0000
		Big // 0x0000FEFF, not implemented !
	};

	struct TextFileInfo
	{
		TextFileFormat m_format;
		TextFileEndian m_endian;
		bool m_hasBOM;
	};

	enum class FileSeekPos : uint32_t {
		Begin,
		Current,
		End
	};

	class File
	{
	public:
		File() {}
		virtual ~File() {}

		virtual void	Flush() = 0;
		virtual TextFileInfo	GetTextFileInfo() = 0;
		virtual void	SetTextFileInfo(TextFileInfo info) = 0;
		virtual uint64_t	Read(uint8_t* data, uint64_t size) = 0;
		virtual void	Seek(uint64_t distance, FileSeekPos pos) = 0;
		virtual uint64_t	Size() = 0;
		virtual uint64_t	Tell() = 0;
		virtual uint32_t	Write(uint8_t* data, uint32_t size) = 0;
		virtual void	Write(const StringA& string) = 0;
		virtual void	Write(const StringW& string) = 0;
		virtual void	Write(const StringU16& string) = 0;
		//virtual void	Write(const kkString32& string) = 0;
	};


	enum class FileAccessMode : uint32_t
	{
		Read,
		Write,
		Both,
		Append
	};

	enum class FileMode : uint32_t
	{
		Text,
		Binary
	};

	enum class FileShareMode : uint32_t
	{
		None,
		Delete,
		Read,
		Write
	};

	enum class FileAction : uint32_t
	{
		Open,
		Open_new,
	};

	enum class FileAttribute : uint32_t
	{
		Normal,
		Hidden,
		Readonly
	};

	class FileSystem
	{
		bool	m_dirScanBegin = false;
		bool	m_firstCall = false;
		HANDLE	hFind = nullptr;
		StringW	m_dir;
		StringW	m_exePath, m_systemPath;

		void	DeleteFolder(const StringW& dir);
	public:

		FileSystem();
		~FileSystem();

		File* CreateFile(
			const StringU16& fileName,
			FileMode mode,
			FileAccessMode access,
			FileAction action,
			FileShareMode shareMode = FileShareMode::None,
			uint32_t attributeFlags = 0u
		);

		bool CopyFile(const StringW& existingFileName, const StringW& newFileName, bool overwrite);
		bool CreateDir(const StringW& dir);
		bool DeleteDir(const StringW& dir);
		bool DeleteFile(const StringW& file);
		bool ExistDir(const StringW& dir);
		
		bool ExistFile(const StringW& file);
		bool ExistFile(const StringA& file);
		bool ExistFile(const StringU16& file);

		StringW GetProgramPath();
		StringW GetRealPath(const StringW& in);
		StringW GetSystemPath();

		enum class DirObjectType : uint32_t
		{
			info,
			folder,
			file
		};

		struct DirObject {
			DirObject() { memset(path, 0u, MGF_MAX_PATH * sizeof(wchar_t)); }
			wchar_t path[MGF_MAX_PATH];
			DirObjectType type;
			uint32_t size;
		};

		bool GetDirObject(DirObject* inout);
		void ScanDirBegin(StringW dir);
		void ScanDirEnd();

		XML::XMLDocument* XMLRead(const char* file);
		XML::XMLDocument* XMLRead(const uint8_t* data, size_t dataSize);
		void XMLWrite(const char* file, XML::XMLNode* rootNode, bool utf8 = false);

	};

	namespace util
	{
		inline File*
			openFileForReadText(const StringU16& fileName) {
			FileSystem fs;
			return fs.CreateFile(fileName, FileMode::Text, FileAccessMode::Read, FileAction::Open);
		}

		inline File* createFileForReadText(const StringU16& fileName) {
			FileSystem fs;
			return fs.CreateFile(fileName, FileMode::Text, FileAccessMode::Read, FileAction::Open_new);
		}

		inline File* openFileForWriteText(const StringU16& fileName) {
			FileSystem fs;
			return fs.CreateFile(fileName, FileMode::Text, FileAccessMode::Append, FileAction::Open);
		}

		inline	File* createFileForWriteText(const StringU16& fileName) {
			FileSystem fs;
			return fs.CreateFile(fileName, FileMode::Text, FileAccessMode::Write, FileAction::Open_new);
		}

		inline File* openFileForReadBin(const StringU16& fileName) {
			FileSystem fs;
			return fs.CreateFile(fileName, FileMode::Binary, FileAccessMode::Read, FileAction::Open);
		}

		inline File* openFileForReadBinShared(const StringU16& fileName) {
			FileSystem fs;
			return fs.CreateFile(fileName, FileMode::Binary, FileAccessMode::Read, FileAction::Open, FileShareMode::Read);
		}

		inline File* openFileForWriteBin(const StringU16& fileName) {
			FileSystem fs;
			return fs.CreateFile(fileName, FileMode::Binary, FileAccessMode::Write, FileAction::Open);
		}

		inline File* createFileForWriteBin(const StringU16& fileName) {
			FileSystem fs;
			return fs.CreateFile(fileName, FileMode::Binary, FileAccessMode::Write, FileAction::Open_new);
		}

		inline File* createFileForWriteBinShared(const StringU16& fileName) {
			FileSystem fs;
			return fs.CreateFile(fileName, FileMode::Binary, FileAccessMode::Write, FileAction::Open_new, FileShareMode::Read);
		}

		inline
			bool
			readTextFromFileForUnicode(const StringU16& fileName, StringU16& utf16)
		{
			File* file = util::openFileForReadBin(fileName);
			if (!file)
			{
				delete file;
				return false;
			}

			uint64_t sz = file->Size();
			if (sz < 4)
			{
				delete file;
				return false;
			}

			uint8_t bom[3u];
			file->Read(bom, 3u);
			file->Seek(0u, FileSeekPos::Begin);

			bool isUTF8 = false;
			bool isBE = false;

			if (bom[0u] == 0xEF) {
				file->Seek(3u, FileSeekPos::Begin);
				isUTF8 = true;
				sz -= 3u;
			}
			else if (bom[0u] == 0xFE) { // utf16 BE
				file->Seek(2u, FileSeekPos::Begin);
				isBE = true;
				sz -= 2u;
			}
			else if (bom[0u] == 0xFF) {
				file->Seek(2u, FileSeekPos::Begin);
				sz -= 2u;
			}
			else {
				// else - utf8 w/o bom
				isUTF8 = true;
			}

			StringA textBytes;
			textBytes.reserve((uint32_t)sz);
			textBytes.setSize((uint32_t)sz);
			file->Read((uint8_t*)textBytes.data(), sz);

			if (!isUTF8) {
				union {
					char16_t unicode;
					char b[2u];
				}un;
				for (uint32_t i = 0u; i < sz; i += 2u) {
					/*char16_t ch16 = textBytes[ i ];
					if( isBE ){
						ch16 <<= kkConst8U;
						ch16 |= textBytes[ i + 1u ];
					}else{
						char16_t ch16_2 = textBytes[ i + 1u ];
						ch16_2 <<= kkConst8U;
						ch16 |= ch16_2;
					}*/

					if (isBE) {
						un.b[0u] = textBytes[i + 1u];
						un.b[1u] = textBytes[i];
					}
					else {
						un.b[0u] = textBytes[i];
						un.b[1u] = textBytes[i + 1u];
					}

					utf16 += un.unicode;
				}

			}
			else
			{
				util::string_UTF8_to_UTF16(utf16, textBytes);
			}
			delete file;
			return true;
		}
	}
}

#endif
