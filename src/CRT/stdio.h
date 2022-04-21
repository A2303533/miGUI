#pragma once
#ifndef __STDIO__
#define __STDIO__

#include "CRT.h"

// ISO/IEC 9899:TC3, N1256

// 7.19.1:2
#include "size_t.h"

#ifndef _FILE_DEFINED
typedef struct {
    void* handle;
    char* tmpfn;
    unsigned int flags;
    unsigned long long pos;

    // I DON't know for what this:
    unsigned char* buf;
    unsigned int fpi;
    unsigned int erri;
    unsigned int eofi;
} FILE;
#define _FILE_DEFINED
#endif

typedef long long fpos_t;

// 7.19.1:3
#include "NULL.h"
#define _IOFBF 1
#define _IOLBF 2
#define _IONBF 3
#define BUFSIZ 512
#define EOF (-1)
#define FOPEN_MAX 20
#define FILENAME_MAX 260

#ifdef _POSIX_
#define _P_tmpdir "/"
#define _wP_tmpdir L"/"
#else
#define _P_tmpdir "\\"
#define _wP_tmpdir L"\\"
#endif

#define L_tmpnam (sizeof(_P_tmpdir) + 12)

#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0

#define TMP_MAX 32767

#define stdin __CRT_stdin()
#define stdout __CRT_stdout()
#define stderr __CRT_stderr()

#ifdef __cplusplus
extern "C" {
#endif

    FILE* _C_DECL __CRT_stdin();
    FILE* _C_DECL __CRT_stdout();
    FILE* _C_DECL __CRT_stderr();

    // 7.19.4.1
    int _C_DECL remove(const char* filename);

    // 7.19.4.2
    int _C_DECL rename(const char* old, const char* _new);

    // 7.19.4.3
    FILE* _C_DECL tmpfile(void);

    // 7.19.4.4
    //  char* name1 = tmpnam(NULL);
    //  printf("temporary file name: %s\n", name1);
    // 
    //  char name2[L_tmpnam];
    //  if (tmpnam(name2))
    //    printf("temporary file name: %s\n", name2);
    char* _C_DECL tmpnam(char*);

#define __CRT_EXST_NEXST 0 //not exist
#define __CRT_EXST_FILE 1 //this is file
#define __CRT_EXST_DIR 2 // or directory
    // return __CRT_EXST_... 
    int _C_DECL __CRT_exist(const char*);
    int _C_DECL __CRT_wexist(const wchar_t*);
    
    // 7.19.5.1
    int _C_DECL fclose(FILE* stream);

    // 7.19.5.2
    int _C_DECL fflush(FILE* stream);

    // 7.19.5.3
    FILE* _C_DECL fopen(const char* filename, const char* mode);

#define __CRT_FOPEN_READ 0x1
#define __CRT_FOPEN_WRITE 0x2

#define __CRT_FOPEN_APPEND 0x4
#define __CRT_FOPEN_TMP 0x8

#define __CRT_FOPEN_CREATION_OPENEXST 0x10    // read
#define __CRT_FOPEN_CREATION_CREATEOPEN 0x20 // append
#define __CRT_FOPEN_CREATION_CREATENEW 0x40  // write

#define __CRT_FOPEN_SHAREMODE_READ 0x80
#define __CRT_FOPEN_SHAREMODE_WRITE 0x100
#define __CRT_FOPEN_ISDEVICE 0x200
    FILE* _C_DECL _CRT_openFile(const char* fn, unsigned int fl);

    // 7.19.5.4
    FILE* _C_DECL freopen(const char* filename, const char* mode, FILE* stream);

    // 7.19.8.2
    size_t _C_DECL fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);

#ifdef __cplusplus
}
#endif

#endif