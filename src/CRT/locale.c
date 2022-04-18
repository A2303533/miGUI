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
			__crt.locale.decimal_point = ".";
			__crt.locale.thousands_sep = "";
			__crt.locale.grouping = "";
			__crt.locale.mon_decimal_point = "";
			__crt.locale.mon_thousands_sep = "";
			__crt.locale.mon_grouping = "";
			__crt.locale.positive_sign = "";
			__crt.locale.negative_sign = "";
			__crt.locale.currency_symbol = "";
			__crt.locale.frac_digits = CHAR_MAX;
			__crt.locale.p_cs_precedes = CHAR_MAX;
			__crt.locale.n_cs_precedes = CHAR_MAX;
			__crt.locale.p_sep_by_space = CHAR_MAX;
			__crt.locale.n_sep_by_space = CHAR_MAX;
			__crt.locale.p_sign_posn = CHAR_MAX;
			__crt.locale.n_sign_posn = CHAR_MAX;
			__crt.locale.int_curr_symbol = "";
			__crt.locale.int_frac_digits = CHAR_MAX;
			__crt.locale.int_p_cs_precedes = CHAR_MAX;
			__crt.locale.int_n_cs_precedes = CHAR_MAX;
			__crt.locale.int_p_sign_posn = CHAR_MAX;
			__crt.locale.int_n_sign_posn = CHAR_MAX;
			__crt.locale.int_p_sign_posn = CHAR_MAX;
			__crt.locale.int_n_sign_posn = CHAR_MAX;
			break;
		}
	}
	return "C";
}

lconv* 
_C_DECL 
localeconv(void)
{
	return &__crt.locale;
}


