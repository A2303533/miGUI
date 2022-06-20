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
#ifndef _MGF_ARCHIVE_H_
#define _MGF_ARCHIVE_H_

namespace mgf
{
    /// <summary>
    /// This is not algorithm.
    /// This is library. If some library will have many algorithms,
    /// here will be more options like:
    ///     megalib_deflate,
    ///     megalib_lz77,
    ///     megalib_lzma,
    /// We have:
    ///     -   fastlz (LZ77)
    /// </summary>
    enum class CompressorType : uint32_t
    {
        // m_level:
        //  1 for speed
        //  2 compression ratio 
        fastlz
    };

    /// <summary>
    /// Minimum iformation for data compression.
    /// </summary>
    struct CompressionInfo
    {
        CompressorType m_compressorType = CompressorType::fastlz;

        /// <summary>
        /// Read the comments below in Compress\Decomress
        /// </summary>
        uint32_t m_sizeUncompressed = 0;
        uint8_t* m_dataUncompressed = 0;

        uint32_t m_sizeCompressed = 0;
        uint8_t* m_dataCompressed = 0;

        /// <summary>
        /// See CompressorType. If the value is wrong, the 
        /// default value will be used.
        /// </summary>
        uint32_t m_level = 2;
    };

    struct ArchiveZipFile
    {
        std::vector<StringA> m_files;

        StringA m_fileName;
    };

    /// <summary>
    /// Not super mega abstract class.
    /// Just simple wrapper class without some initializations
    /// or memory allocations.
    /// In future here must be a std::vector that will have pointers
    /// to class for `zip` and other archive files.
    /// </summary>
    class ArchiveSystem : public BaseClass
    {
        bool _compress_fastlz(CompressionInfo* info);
        bool _decompress_fastlz(CompressionInfo* info);
       // bool _decompress_fastlz(CompressionInfo* info, std::vector<uint8_t>& toVector);

        std::vector<ArchiveZipFile*> m_zipFiles;
    public:
        ArchiveSystem();
        ~ArchiveSystem();

        /// <summary>
        /// Add zip file for unzipping files from it.
        /// </summary>
        /// <param name="zipFile">Path to .zip file</param>
        /// <returns>true - if success</returns>
        bool ZipAdd(const char* zipFile);

        /// <summary>
        /// Check if file inside of some .zip archive.
        /// </summary>
        /// <param name="fileInZip">File path.</param>
        /// <returns>If some .zip file contain this file then return address.</returns>
        ArchiveZipFile* ZipContain(const char* fileInZip);

        /// <summary>
        /// Unzip file. If a is NULL first found file will be processed.
        /// Set a if you have many zip files with same file inside (use ZipContain to find a).
        /// Function will allocate memory using malloc, and will write data size in size.
        /// Pointer will return if everything is OK, use free for deallocation.
        /// </summary>
        /// <param name="fileInZip">File path.</param>
        uint8_t* ZipUnzip(const char* fileInZip, uint32_t* size, ArchiveZipFile* a = NULL);

        /// <summary>
        /// Set all data in `info`. If compression is OK, true will return
        /// and m_dataCompressed will have address.
        /// You need to call free(m_dataCompressed) for deallocation.
        /// </summary>
        /// <returns>true - if success</returns>
        bool Compress(CompressionInfo* info);

        /// <summary>
        /// Like in `Compress` method.
        /// Allocate memory m_dataUncompressed by yourself.
        /// You must know m_sizeUncompressed.
        /// </summary>
        /// <returns>true - if success</returns>
        bool Decompress(CompressionInfo* info);
        
        /// <summary>
        /// Decompress and store data in std::vector.
        /// if info.m_dataCompressed is NULL (or !info.m_sizeCompressed)
        /// then function will try to copy data from info.m_dataUncompressed
        /// Function will allocate memory, put all data to std::vector and then
        /// will free memory.
        /// info.m_dataUncompressed and info.m_sizeUncompressed will be ignored
        /// </summary>
        /// <returns>true - if success</returns>
        bool Decompress(CompressionInfo* info, std::vector<uint8_t>& toVector);
    };
}

#endif
