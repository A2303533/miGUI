#pragma once

#ifndef NULL
#ifdef __cplusplus
#if (__cplusplus > 199711L) /*MSVS compiler option: /Zc:__cplusplus*/
#define NULL nullptr
#else
#define NULL 0
#endif
#else
#define NULL ((void *)0)
#endif
#endif