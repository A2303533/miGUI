#pragma once
#ifndef __ERRNO__
#define __ERRNO__

#include "CRT.h"


/*ISO/IEC 9899:TC3, N1256*/

/*7.5:2*/
#define EDOM 1   // bad arg
#define EILSEQ 2 // Illegal byte sequence
#define ERANGE 3 // Result too large
#define errno errno


#endif