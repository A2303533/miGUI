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

#include <string>

namespace mgf
{
    namespace filesystem
    {
        enum class file_type 
        {
            none = 0,
            not_found,
            regular,
            directory,
            symlink,
            block,
            character,
            fifo,
            socket,
            unknown,
        };

        enum class perms
        {
            none = 0,//​		no permission bits are set
            owner_read = 0400,  //	S_IRUSR	File owner has read permission
            owner_write = 0200, //	S_IWUSR	File owner has write permission
            owner_exec = 0100,  //	S_IXUSR	File owner has execute / search permission
            owner_all = 0700,   //	S_IRWXU	File owner has read, write, and execute / search permissions
                                //  Equivalent to owner_read | owner_write | owner_exec

            group_read = 040,  //	S_IRGRP	The file's user group has read permission
            group_write	= 020, //	S_IWGRP	The file's user group has write permission
            group_exec = 010,  //	S_IXGRP	The file's user group has execute/search permission
            group_all = 070,   //	S_IRWXG	The file's user group has read, write, and execute/search permissions
                               //   Equivalent to group_read | group_write | group_exec

            others_read	= 04,  //	S_IROTH	Other users have read permission
            others_write = 02, //	S_IWOTH	Other users have write permission
            others_exec = 01,  //	S_IXOTH	Other users have execute / search permission
            others_all = 07,   //	S_IRWXO	Other users have read, write, and execute / search permissions
                               //   Equivalent to others_read | others_write | others_exec

            all	= 0777,        //		All users have read, write, and execute / search permissions
                               // Equivalent to owner_all | group_all | others_all

            set_uid	= 04000,   //	S_ISUID	Set user ID to file owner user ID on execution
            set_gid	= 02000,   // 	S_ISGID	Set group ID to file's user group ID on execution
            sticky_bit = 01000,//	S_ISVTX	Implementation - defined meaning, but POSIX XSI specifies that when set on a directory, only file owners may delete files even if the directory is writeable to others(used with / tmp)
            mask = 07777,      //		All valid permission bits.
                               // Equivalent to all | set_uid | set_gid | sticky_bit

            unknown	= 0xFFFF,  //	Unknown permissions(e.g.when filesystem::file_status is created without permissions)
        };

        class file_status
        {
            file_type m_type = file_type::none;
            perms m_perm = perms::none;
        public:
            file_status() {}
            ~file_status() {}

            file_type type() const MGF_NOEXCEPT { return m_type; }
            void type(file_type type) MGF_NOEXCEPT { m_type = type; };

            perms permissions() const MGF_NOEXCEPT { return m_perm; }
            void permissions(perms perm) MGF_NOEXCEPT { m_perm = perm; }
        };

        class path
        {
        public:
            typedef wchar_t value_type;
            typedef std::basic_string<value_type> string_type;

            path() {};
            ~path() {};
            
            path(const char* str) { assign(str); };
            path(const wchar_t* str) { assign(str); };

            path& assign(const char* str);
            path& assign(const wchar_t* str)
            {
                m_string = str;
                return *this;
            }

            const value_type* c_str() const MGF_NOEXCEPT { return m_string.c_str(); }
            const string_type& native() const MGF_NOEXCEPT { return m_string; }
            operator string_type() const { return m_string; }
            
            std::string string() const;
            std::wstring wstring() const;

            path filename() const;
            path extension() const;

        private:
            string_type m_string;
        };

        bool exists(const path& p);
        bool is_regular_file(const path& p);
        std::uintmax_t file_size(const path& p);
    }
}


#endif