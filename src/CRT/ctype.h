#pragma once
#ifndef __CTYPE__
#define __CTYPE__

#include "CRT.h"

/*ISO/IEC 9899:TC3, N1256*/

#ifdef __cplusplus
extern "C" {
#endif

	/*7.4.1.1*/
	/// <summary>
	/// Return 1 if character is digit or character (basic latin alphabet).
	/// </summary>
	/// <param name="c" >- character</param>
	/// <returns>1 if true</returns>
	int _C_DECL isalnum(int c);

	/*7.4.1.2*/
	/// <summary>
	/// Return 1 if character is character (basic latin alphabet).
	/// </summary>
	/// <param name="c" >- character</param>
	/// <returns>1 if true</returns>
	int _C_DECL isalpha(int c);

	/*7.4.1.3*/
	int _C_DECL isblank(int c);

	/*7.4.1.4*/
	int _C_DECL iscntrl(int c);

	/*7.4.1.5*/
	int _C_DECL isdigit(int c);

	/*7.4.1.6*/
	int _C_DECL isgraph(int c);

	/*7.4.1.7*/
	int _C_DECL islower(int c);

	/*7.4.1.8*/
	int _C_DECL isprint(int c);

	/*7.4.1.9*/
	int _C_DECL ispunct(int c);

	/*7.4.1.10*/
	int _C_DECL isspace(int c);

	/*7.4.1.11*/
	int _C_DECL isupper(int c);

	/*7.4.1.12*/
	int _C_DECL isxdigit(int c);

	/*7.4.2.1*/
	int _C_DECL tolower(int c);

	/*7.4.2.2*/
	int _C_DECL toupper(int c);

#ifdef __cplusplus
}
#endif


#endif