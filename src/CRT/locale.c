#include "CRT.h"
//#include "stdlib.h"
#include "locale.h"
#include "string.h"
#include "limits.h"

extern __CRT_main_struct __crt;

char* 
_C_DECL 
setlocale(int category, const char* locale)
{
	if (strcmp(locale, "C") == 0)
	{
		switch (category)
		{
		default:
		case LC_ALL:
			__crt._->locale.decimal_point = ".";
			__crt._->locale.thousands_sep = "";
			__crt._->locale.grouping = "";
			__crt._->locale.mon_decimal_point = "";
			__crt._->locale.mon_thousands_sep = "";
			__crt._->locale.mon_grouping = "";
			__crt._->locale.positive_sign = "";
			__crt._->locale.negative_sign = "";
			__crt._->locale.currency_symbol = "";
			__crt._->locale.frac_digits = CHAR_MAX;
			__crt._->locale.p_cs_precedes = CHAR_MAX;
			__crt._->locale.n_cs_precedes = CHAR_MAX;
			__crt._->locale.p_sep_by_space = CHAR_MAX;
			__crt._->locale.n_sep_by_space = CHAR_MAX;
			__crt._->locale.p_sign_posn = CHAR_MAX;
			__crt._->locale.n_sign_posn = CHAR_MAX;
			__crt._->locale.int_curr_symbol = "";
			__crt._->locale.int_frac_digits = CHAR_MAX;
			__crt._->locale.int_p_cs_precedes = CHAR_MAX;
			__crt._->locale.int_n_cs_precedes = CHAR_MAX;
			__crt._->locale.int_p_sign_posn = CHAR_MAX;
			__crt._->locale.int_n_sign_posn = CHAR_MAX;
			__crt._->locale.int_p_sign_posn = CHAR_MAX;
			__crt._->locale.int_n_sign_posn = CHAR_MAX;
			break;
		}
	}
	return "C";
}

struct lconv* 
_C_DECL 
localeconv(void)
{
	return &__crt._->locale;
}


