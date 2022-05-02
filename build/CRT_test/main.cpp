#include "CRT/assert.h"
#include "CRT/stdlib.h"
#include "CRT/stdio.h"
#include "CRT/string.h"
#include "CRT/stdarg.h"
#include "CRT/math.h"
#include "CRT/pool.h"
#include "CRT/WinAPI.h"
#include "CRT/ctype.h"

#include <vcruntime_typeinfo.h>
type_info::~type_info() noexcept
{
}
void __cdecl operator delete(void*, unsigned int) {}



//#define SUCCESS 0
//#define FAILURE -1

//int  test_pool(int element_size, int block_size)
//{
//	pool pool_ptr;
//	int* test_ptr1 = NULL;
//	int* test_ptr2 = NULL;
//
//	/* init memory pool with given parameters */
//	poolInitialize(&pool_ptr, element_size, block_size);
//
//	/* allocate memory from memory pool */
//	test_ptr1 = (int*)poolMalloc(&pool_ptr);
//	test_ptr2 = (int*)poolMalloc(&pool_ptr);
//
//	/* test allocated memory validity */
//	if (!(test_ptr1 && test_ptr2))
//	{
//		//printf("memory allocation failure \n");
//
//		return FAILURE;
//	}
//
//	/* free memory allocated from memory pool */
//	poolFree(&pool_ptr, test_ptr1);
//
//	/* free all memory used by this pool */
//	poolFreePool(&pool_ptr);
//
//	return SUCCESS;
//}

void __CRT_main()
{
	__CRT_init();

	//fwrite("Hello\n", 1, 6, stdout);
	//printf("One[%i] Two[%d]\n");

	double f = 3.112345E-03;
	//f = NAN;
	//f = INFINITY;
	//f = 3.112345E-03;
	//if (isnan(f))
	//	printf("NAN\n");
	//if (isinf(f))
	//	printf("INF\n");
	//if (isfinite(f))
	//	printf("FINITE\n");

	printf("FFmpeg is a powerful tool that can do almost anything you can \
imagine with multimedia files. In this article, we are interested in using \
it to convert files, so we won't be taking a deep dive into its entire feat\
ure set. Before we look at using FFmpeg, first we need to take a quick look\
 at what a media file exactly is.**\nHello %i %s\n char [%c]\nUnsigned -1: [%u]\n\
Octal 1234: [%o]\nhex 1234: [%x]\nhex 5678: [%X]\n\
Double: %f\n\
Float: %f\n\
char: %c\n\
int: %i\n\
sciencific: %e\n", 0xf, "World!", '$', -1, 1234, 1234, 5678, f, f, '#', 444, 30.1234);
	printf("Try NAN: [%f] [%F]\n", NAN, NAN);
	printf("Try INFINITY: [%f] [%F]\n", INFINITY, INFINITY);
	float testFloat = 0.f;
	printf("Test float [%f]:\n\tisnan[%i]\n\tisinf[%i]\n\tisfinit[%i]\n\tisnormal[%i]\n\tsignbit[%i]\n", 
		testFloat,
		isnan(testFloat),
		isinf(testFloat), 
		isfinite(testFloat),
		isnormal(testFloat),
		signbit(testFloat)
	);
	testFloat = 10.f;
	printf("Test float [%f]:\n\tisnan[%i]\n\tisinf[%i]\n\tisfinit[%i]\n\tisnormal[%i]\n\tsignbit[%i]\n",
		testFloat,
		isnan(testFloat),
		isinf(testFloat),
		isfinite(testFloat),
		isnormal(testFloat),
		signbit(testFloat)
	);
	testFloat = -10.f;
	printf("Test float [%f]:\n\tisnan[%i]\n\tisinf[%i]\n\tisfinit[%i]\n\tisnormal[%i]\n\tsignbit[%i]\n",
		testFloat,
		isnan(testFloat),
		isinf(testFloat),
		isfinite(testFloat),
		isnormal(testFloat),
		signbit(testFloat)
	);
	testFloat = NAN;
	printf("Test float [%f]:\n\tisnan[%i]\n\tisinf[%i]\n\tisfinit[%i]\n\tisnormal[%i]\n\tsignbit[%i]\n",
		testFloat,
		isnan(testFloat),
		isinf(testFloat),
		isfinite(testFloat),
		isnormal(testFloat),
		signbit(testFloat)
	);
	testFloat = INFINITY;
	printf("Test float [%f]:\n\tisnan[%i]\n\tisinf[%i]\n\tisfinit[%i]\n\tisnormal[%i]\n\tsignbit[%i]\n",
		testFloat,
		isnan(testFloat),
		isinf(testFloat),
		isfinite(testFloat),
		isnormal(testFloat),
		signbit(testFloat)
	);

	testFloat = 0.f;
	printf("acos(%f): [%f]\n", testFloat, acos(testFloat));
	testFloat = 10.f;
	printf("acos(%f): [%f]\n", testFloat, acos(testFloat));
	testFloat = 0.4;
	printf("acos(%f): [%f]\n", testFloat, acos(testFloat));
	testFloat = -1.f;
	printf("acos(%f): [%f]\n", testFloat, acos(testFloat));
	testFloat = 1.f;
	printf("acos(%f): [%f]\n", testFloat, acos(testFloat));
	
	testFloat = 0.f;
	printf("Test sqrt");
	for (int i = 0; i < 2000000; ++i)
	{
		double r = sqrt(i);
		if (isfinite(r))
			testFloat += r * 0.0001f;
	}
	printf(": [%f]\n", testFloat);
	testFloat = 0.f;
	printf("Test sin 1");
	for (int i = 0; i < 2000000; ++i)
	{
		double r = sin(i);
		if (isfinite(r))
			testFloat += r * 0.0001f;
	}
	printf(": [%f]\n", testFloat);
	testFloat = 0.f;
	printf("Test sin 2");
	for (int i = 0; i < 2000000; ++i)
	{
		double r = sinl(i);
		if (isfinite(r))
			testFloat += r * 0.0001f;
	}
	printf(": [%f]\n", testFloat);

	testFloat = 0.f;
	printf("sqrt(%f): [%f]\n", testFloat, sqrt(testFloat));
	testFloat = 10.f;
	printf("sqrt(%f): [%f]\n", testFloat, sqrt(testFloat));
	testFloat = 0.4;
	printf("sqrt(%f): [%f]\n", testFloat, sqrt(testFloat));
	testFloat = -1.f;
	printf("sqrt(%f): [%f]\n", testFloat, sqrt(testFloat));
	testFloat = 1.f;
	printf("sqrt(%f): [%f]\n", testFloat, sqrt(testFloat));

	testFloat = 0.f;
	printf("fabs(%f): [%f]\n", testFloat, fabs(testFloat));
	testFloat = 1.f;
	printf("fabs(%f): [%f]\n", testFloat, fabs(testFloat));
	testFloat = -1.f;
	printf("fabs(%f): [%f]\n", testFloat, fabs(testFloat));

	testFloat = 0.f;
	printf("fabs(%f): [%f]\n", testFloat, fabs(testFloat));
	testFloat = 1.f;
	printf("fabs(%f): [%f]\n", testFloat, fabs(testFloat));
	testFloat = -1.f;
	printf("fabs(%f): [%f]\n", testFloat, fabs(testFloat));

	testFloat = 0.f;
	printf("asin(%f): [%f]\n", testFloat, asin(testFloat));
	testFloat = 1.f;
	printf("asin(%f): [%f]\n", testFloat, asin(testFloat));
	testFloat = 0.5f;
	printf("asin(%f): [%f]\n", testFloat, asin(testFloat));
	testFloat = -0.5f;
	printf("asin(%f): [%f]\n", testFloat, asin(testFloat));

	testFloat = 0.f;
	printf("tan(%f): [%f]\n", testFloat, tan(testFloat));
	testFloat = 0.5f;
	printf("tan(%f): [%f]\n", testFloat, tan(testFloat));
	testFloat = 1.f;
	printf("tan(%f): [%f]\n", testFloat, tan(testFloat));
	testFloat = -0.5f;
	printf("tan(%f): [%f]\n", testFloat, tan(testFloat));
	testFloat = -1.f;
	printf("tan(%f): [%f]\n", testFloat, tan(testFloat));

	testFloat = 0.f;
	printf("atan(%f): [%f]\n", testFloat, atan(testFloat));
	testFloat = 0.5f;
	printf("atan(%f): [%f]\n", testFloat, atan(testFloat));
	testFloat = 1.f;
	printf("atan(%f): [%f]\n", testFloat, atan(testFloat));
	testFloat = -0.5f;
	printf("atan(%f): [%f]\n", testFloat, atan(testFloat));
	testFloat = -1.f;
	printf("atan(%f): [%f]\n", testFloat, atan(testFloat));

	testFloat = 0.f;
	printf("floor(%f): [%f]\n", testFloat, floor(testFloat));
	testFloat = 0.5f;
	printf("floor(%f): [%f]\n", testFloat, floor(testFloat));

	double x = 5.f, y = 3.f;
	printf("atan2(%f, %f): [%f]\n", x, y, atan2(y, x));
	x = 15.f, y = 3.f;
	printf("atan2(%f, %f): [%f]\n", x, y, atan2(y, x));
	x = 5.f, y = 13.f;
	printf("atan2(%f, %f): [%f]\n", x, y, atan2(y, x));

	testFloat = 0.5f;
	printf("sin(%f): [%f]\n", testFloat, sin(testFloat));
	testFloat = -0.5f;
	printf("sin(%f): [%f]\n", testFloat, sin(testFloat));

	testFloat = 0.5f;
	printf("cos(%f): [%f]\n", testFloat, cos(testFloat));
	testFloat = -0.5f;
	printf("cos(%f): [%f]\n", testFloat, cos(testFloat));

	testFloat = 2.f;
	printf("acosh(%f): [%f]\n", testFloat, acosh(testFloat));
	testFloat = 1.f;
	printf("acosh(%f): [%f]\n", testFloat, acosh(testFloat));
	testFloat = 0.f;
	printf("acosh(%f): [%f]\n", testFloat, acosh(testFloat));

	testFloat = 2.f;
	printf("asinh(%f): [%f]\n", testFloat, asinh(testFloat));
	testFloat = 1.f;
	printf("asinh(%f): [%f]\n", testFloat, asinh(testFloat));
	testFloat = 0.f;
	printf("asinh(%f): [%f]\n", testFloat, asinh(testFloat));

	testFloat = 100.f;
	printf("log(%f): [%f]\n", testFloat, log(testFloat));
	testFloat = 40.f;
	printf("log(%f): [%f]\n", testFloat, log(testFloat));
	testFloat = -4.f;
	printf("log(%f): [%f]\n", testFloat, log(testFloat));

	testFloat = 1.f;
	printf("log1p(%f): [%f]\n", testFloat, log1p(testFloat));
	testFloat = 2.f;
	printf("log1p(%f): [%f]\n", testFloat, log1p(testFloat));
	testFloat = 3.f;
	printf("log1p(%f): [%f]\n", testFloat, log1p(testFloat));

	testFloat = 0.6f;
	printf("atanh(%f): [%f]\n", testFloat, atanh(testFloat));
	testFloat = -0.5f;
	printf("atanh(%f): [%f]\n", testFloat, atanh(testFloat));
	testFloat = -2.f;
	printf("atanh(%f): [%%f]\n", testFloat, atanh(testFloat));
	
	testFloat = -2.09f;
	printf("lrint(%i): [%i]\n", lrint(testFloat), (int)testFloat);

	printf("Characters: [%3c] %c %lc\n", 'a', 65, 66);
	printf("Decimals: %d %ld %lli\n", 1977, 650000L, 0xFFFFFFFFFF);
	printf("Decimals2: %01d %020lli\n", 444, 0xfffffffffffff);
	printf("Preceding with blanks: %10d \n", 1977);
	printf("Preceding with zeros: %010d \n", 1977);
	printf("Some different radices: %d %x %o %#x %#o \n", 100, 100, 100, 100, 100);

	__CRT_shutdown();
}
