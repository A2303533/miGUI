#pragma once
#ifndef __LOCALE__
#define __LOCALE__

#include "CRT.h"

//ISO/IEC 9899:TC3, N1256

/*7.11:2*/
struct lconv
{
	char* decimal_point;     // "."
	char* thousands_sep;     // ""
	char* grouping;          // ""
	char* mon_decimal_point; // ""
	char* mon_thousands_sep; // ""
	char* mon_grouping;      // ""
	char* positive_sign;     // ""
	char* negative_sign;     // ""
	char* currency_symbol;   // ""
	char frac_digits;        // CHAR_MAX
	char p_cs_precedes;      // CHAR_MAX
	char n_cs_precedes;      // CHAR_MAX
	char p_sep_by_space;     // CHAR_MAX
	char n_sep_by_space;     // CHAR_MAX
	char p_sign_posn;        // CHAR_MAX
	char n_sign_posn;        // CHAR_MAX
	char* int_curr_symbol;   // ""
	char int_frac_digits;    // CHAR_MAX
	char int_p_cs_precedes;  // CHAR_MAX
	char int_n_cs_precedes;  // CHAR_MAX
	char int_p_sep_by_space; // CHAR_MAX
	char int_n_sep_by_space; // CHAR_MAX
	char int_p_sign_posn;    // CHAR_MAX
	char int_n_sign_posn;    // CHAR_MAX
};

/*7.11:3*/
#define LC_ALL 0
#define LC_COLLATE 1
#define LC_CTYPE 2
#define LC_MONETARY 3
#define LC_NUMERIC 4
#define LC_TIME 5

#ifdef __cplusplus
extern "C" {
#endif

	/*7.11.1.1*/
	char* _C_DECL setlocale(int category, const char* locale);

	/*7.11.2.1*/
	struct lconv* _C_DECL localeconv(void);

#ifdef __cplusplus
}
#endif


#endif