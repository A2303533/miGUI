#pragma once

#ifndef __CRT_SIZE_T_DEFINED
#define __CRT_SIZE_T_DEFINED
#ifdef __CRT_WIN32
#ifndef _WIN64
typedef unsigned int size_t;
#else
typedef unsigned long long size_t;
#endif
#else
typedef unsigned int size_t;
#endif /*__CRT_WIN32*/
#endif