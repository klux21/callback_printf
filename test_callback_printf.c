#if 0
cc -Wall -ggdb -o _test_callback_printf -I . test_callback_printf.c callback_printf.c
./_test_callback_printf
exit $?
#endif

/*****************************************************************************\
*                                                                             *
*  FILENAME:      test_callback_printf.c                                      *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  DESCRIPTION:   tests of callback_printf related functions                  *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  COPYRIGHT:     (c) 2023 Dipl.-Ing. Klaus Lux (Aachen, Germany)             *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  REPOSITORY:     https://github/klux21/callback_printf                      *
*                                                                             *
\*****************************************************************************/

#include <stdio.h>
#include <stddef.h>
#include <signal.h>
#include <float.h>
#include <string.h>
#include <inttypes.h>
#include <callback_printf.h>

/* int test_svsprintf(const char * pout, const char * call, const char * pfmt, ...)  __PRINTF_LIKE_ARGS (3, 4); */

int test_svsprintf(const char * pout, const char * call, const char * pfmt, ...)
{
    int bRet = 0;
    size_t sRet;
    size_t snRet;
    va_list VarArgs;
    char buf[2048];
    char buf2[2048];

    memset(buf, 0xfefefefe,  sizeof(buf));
    memset(buf2, 0xfefefefe, sizeof(buf));

    va_start(VarArgs, pfmt);
    sRet = svsprintf(buf, pfmt, VarArgs);
    va_end(VarArgs);

    va_start(VarArgs, pfmt);
    snRet = svsnprintf(buf2, 17, pfmt, VarArgs);
    va_end(VarArgs);

    if(sRet != snRet)
    {
       printf("test_callback_printf: svsprintf output length %zd (%s) for '%s' does not match retval %zd of svsnprintf!\n", sRet, buf, call, snRet);
       goto Exit;
    }

    if(buf2[17] != '\xfe')
    {
       buf2[24] = '\0'; 
       printf("test_callback_printf: svsnprintf returned %zd (%s) for '%s' but touched data behind the provided buffer!\n", snRet, buf2, call);
       goto Exit;
    }

    if(buf[sRet+1] != '\xfe')
    {
       buf2[24] = '\0';
       printf("test_callback_printf: svsprintf returned %zd (%s) for '%s' but touched data behind the string end!\n", sRet, buf, call);
       goto Exit;
    }

    if(strncmp(buf, buf2, 17))
    {
       printf("test_callback_printf: written data of svsprintf (%zd bytes: '%.17s') and svsnprintf (%zd bytes: '%.17s')) differ for '%s'!\n", sRet, buf, snRet, buf2, call);
       goto Exit;
    }

#if DEBUG
    va_start(VarArgs, pfmt);
    vsprintf(buf2, pfmt, VarArgs); /* for comparison of data with vsprintf in a debugger */
    va_end(VarArgs);
#endif

    if(sRet != strlen(buf))
    {
       printf("test_callback_printf: output length %zd (%s) for '%s' does not match retval %zd !\n", strlen(buf), buf, call, sRet);
       goto Exit;
    }

    if(strcmp(buf, pout))
    {
       printf("test_callback_printf: output '%s' to '%s' doesn't match expected '%s' !\n", buf, call, pout);
       goto Exit;
    }
    
    bRet = 1;
    Exit:;
    return (bRet);
} /* int test_svsprintf(...) */



double sqrt(double val);
int test_ssprintf()
{
   int bRet = 1;

   static uint32_t inf  = 0x7f800000;
   static uint32_t ninf = 0xff800000;
   static uint32_t nan  = 0x7fc00000;
   static uint32_t nnan = 0xffc00000;

#define DOUBLE_ARG      double
#define LONG_DOUBLE_ARG long double
#define ULONGLONG_ARG   unsigned long long
#define INTMAX_ARG      intmax_t
#define INT_ARG         int
#define SIZE_T_ARG      size_t
#define PTRDIFF_T_ARG   ptrdiff_t
#define PTR_ARG         void *
#define PCHAR_ARG       char *
#define PWCHAR_ARG      wchar_t *
#define NO_ARG          int
#define ARG(x)          ,x
#define NOP(x)          x

#define TEST_CBKPRINTF(pfmt, pout, arg, val) bRet &= test_svsprintf(pout, #pfmt ":" #arg ":" #val, pfmt, (arg) val);

    TEST_CBKPRINTF( "%+#23.15e",  " +1.000000000000000e-01", DOUBLE_ARG, 1.0e-1 );
    TEST_CBKPRINTF( "%+#23.15e",  " +3.900000000000000e+00", DOUBLE_ARG, 3.9 );
    TEST_CBKPRINTF( "%+#23.14e",  " +7.89456123000000e-307", DOUBLE_ARG, 7.89456123e-307 );
    TEST_CBKPRINTF( "%+#23.14e",  " +7.89456123000000e+307", DOUBLE_ARG, 7.89456123e+307 );
    TEST_CBKPRINTF( "%+#23.15e",  " +7.894561230000000e+08", DOUBLE_ARG, 789456123.0 );
    TEST_CBKPRINTF( "%-#23.15e",  "7.894561230000000e+08  ", DOUBLE_ARG, 789456123.0 );
    TEST_CBKPRINTF( "%#23.15e",   "  7.894561230000000e+08", DOUBLE_ARG,789456123.0 );
    TEST_CBKPRINTF( "%#1.1g",     "8.e+08", DOUBLE_ARG, 789456123.0 );
    TEST_CBKPRINTF( "%+#23.15Le",  " +1.000000000000000e-01", LONG_DOUBLE_ARG, 1.0e-1 );
    TEST_CBKPRINTF( "%+#23.15Le",  " +3.900000000000000e+00", LONG_DOUBLE_ARG, 3.9 );
    TEST_CBKPRINTF( "%+#23.14Le",  " +7.89456123000000e-307", LONG_DOUBLE_ARG, 7.89456123e-307 );
    TEST_CBKPRINTF( "%+#23.14Le",  " +7.89456123000000e+307", LONG_DOUBLE_ARG, 7.89456123e+307 );
    TEST_CBKPRINTF( "%+#23.15Le",  " +7.894561230000000e+08", LONG_DOUBLE_ARG, 789456123.0 );
    TEST_CBKPRINTF( "%-#23.15Le",  "7.894561230000000e+08  ", LONG_DOUBLE_ARG, 789456123.0 );
    TEST_CBKPRINTF( "%#23.15Le",   "  7.894561230000000e+08", LONG_DOUBLE_ARG, 789456123.0 );
    TEST_CBKPRINTF( "%#1.1Lg",     "8.e+08",                  LONG_DOUBLE_ARG, 789456123.0 );
    TEST_CBKPRINTF( "%I64d",       "-8589934591",             ULONGLONG_ARG, ((unsigned long long)0xffffffff)*0xffffffff );
    TEST_CBKPRINTF( "%+8I64d",     "    +100",                ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "%+.8I64d",    "+00000100",               ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "%+10.8I64d",  " +00000100",              ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "%_1I64d",     "",                        ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "%-1.5I64d",   "-00100",                  ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "%5I64d",      "  100",                   ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "%5I64d",      " -100",                   ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "%-5I64d",     "100  ",                   ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "%-5I64d",     "-100 ",                   ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "%-.5I64d",    "00100",                   ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "%-.5I64d",    "-00100",                  ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "%-8.5I64d",   "00100   ",                ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "%-8.5I64d",   "-00100  ",                ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "%05I64d",     "00100",                   ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "%05I64d",     "-0100",                   ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "% I64d",      " 100",                    ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "% I64d",      "-100",                    ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "% 5I64d",     "  100",                   ULONGLONG_ARG,  100 );
    TEST_CBKPRINTF( "% 5I64d",     " -100",                   ULONGLONG_ARG,  -100 );
    TEST_CBKPRINTF( "% .5I64d",    " 00100",                  ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "% .5I64d",    "-00100",                  ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "% 8.5I64d",   "   00100",                ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "% 8.5I64d",   "  -00100",                ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "%.0I64d",     "",                        ULONGLONG_ARG, 0 );
    TEST_CBKPRINTF( "%#+21.18I64x",  " 0x00ffffffffffffff9c",     ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "%#.25I64o",     "0001777777777777777777634", ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "%#+24.20I64o",  " 01777777777777777777634",  ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "%#+18.21I64X",  "0X00000FFFFFFFFFFFFFF9C",   ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "%#+20.24I64o",  "001777777777777777777634",  ULONGLONG_ARG, -100 );
    TEST_CBKPRINTF( "%#+25.22I64u",  "   0018446744073709551615", ULONGLONG_ARG, -1 );
    TEST_CBKPRINTF( "%#+25.22I64u",  "   0018446744073709551615", ULONGLONG_ARG, -1 );
    TEST_CBKPRINTF( "%#+30.25I64u",  "     0000018446744073709551615", ULONGLONG_ARG, -1 );
    TEST_CBKPRINTF( "%+#25.22I64d",  "  -0000000000000000000001", ULONGLONG_ARG, -1 );
    TEST_CBKPRINTF( "%#-8.5I64o",    "00144   ",                  ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "%#-+ 08.5I64d", "+00100  ",                  ULONGLONG_ARG, 100 );
    TEST_CBKPRINTF( "%.80I64d",      "00000000000000000000000000000000000000000000000000000000000000000000000000000001",  ULONGLONG_ARG, 1 );
    TEST_CBKPRINTF( "% .80I64d",     " 00000000000000000000000000000000000000000000000000000000000000000000000000000001", ULONGLONG_ARG, 1 ); 
    TEST_CBKPRINTF( "% .80d",        " 00000000000000000000000000000000000000000000000000000000000000000000000000000001", INT_ARG, 1 );
    TEST_CBKPRINTF( "%I",            "" , INT_ARG, 1 );
    TEST_CBKPRINTF( "%Iq",           "" , INT_ARG, 1 );
    TEST_CBKPRINTF( "%Ihd",          "" , INT_ARG, 1 );
    TEST_CBKPRINTF( "%I0d",          "" , INT_ARG, 1 );
    TEST_CBKPRINTF( "%I64D",         "" , ULONGLONG_ARG, -1 );
    TEST_CBKPRINTF( "%zx",           "1",   SIZE_T_ARG, 1 );
    TEST_CBKPRINTF( "%z",            "" ,   SIZE_T_ARG, 1 );
    TEST_CBKPRINTF( "%tx",           "1",   PTRDIFF_T_ARG, 1 );
    TEST_CBKPRINTF( "%t",            "" ,   PTRDIFF_T_ARG, 1 );
    TEST_CBKPRINTF( "% d",           " 1",  INT_ARG, 1 );
    TEST_CBKPRINTF( "%+ d",          "+1",  INT_ARG, 1 );
    TEST_CBKPRINTF( "%S",            "wide", PWCHAR_ARG, L"wide" );
    TEST_CBKPRINTF( "%04c",          "   1", INT_ARG, '1' );
    TEST_CBKPRINTF( "%-04c",         "1   ", INT_ARG, '1' );
    TEST_CBKPRINTF( "%#012x",        "0x0000000001", INT_ARG, 1 );
    TEST_CBKPRINTF( "%#012x",        "000000000000", INT_ARG, 0 );
    TEST_CBKPRINTF( "%#04.8x",       "0x00000001",   INT_ARG, 1 );
    TEST_CBKPRINTF( "%#04.8x",       "00000000",     INT_ARG, 0 );
    TEST_CBKPRINTF( "%#-08.2x",      "0x01    ",     INT_ARG, 1 );
    TEST_CBKPRINTF( "%#-08.2x",      "00      ",     INT_ARG, 0 );
    TEST_CBKPRINTF( "%#.0x",         "0x1",          INT_ARG, 1 );
    TEST_CBKPRINTF( "%#.0x",         "",            INT_ARG, 0 );
    TEST_CBKPRINTF( "%#08o",         "00000001",    INT_ARG, 1 );
    TEST_CBKPRINTF( "%#o",           "01",          INT_ARG, 1 );
    TEST_CBKPRINTF( "%#o",           "0",           INT_ARG, 0 );
    TEST_CBKPRINTF( "%04s",          " foo",        PCHAR_ARG, "foo" );
    TEST_CBKPRINTF( "%.1s",          "f",           PCHAR_ARG, "foo" );
    TEST_CBKPRINTF( "%.0s",          "",            PCHAR_ARG, "foo" );
    TEST_CBKPRINTF( "hello",         "hello",       NO_ARG, 0 );
    TEST_CBKPRINTF( "%ws",           "",            PWCHAR_ARG, L"wide" );
    TEST_CBKPRINTF( "%-10ws",        "",            PWCHAR_ARG, L"wide" );
    TEST_CBKPRINTF( "%10ws",         "",            PWCHAR_ARG, L"wide" );
    TEST_CBKPRINTF( "%#+ -03whlls",  "",            PWCHAR_ARG, L"wide" );
    TEST_CBKPRINTF( "%w0s",          "",            PWCHAR_ARG, L"wide" );
    TEST_CBKPRINTF( "%w-s",          "",            PWCHAR_ARG, L"wide" );
    TEST_CBKPRINTF( "%ls",           "wide",        PWCHAR_ARG, L"wide" );
    TEST_CBKPRINTF( "%Ls",           "",            PWCHAR_ARG, "not wide" );
    TEST_CBKPRINTF( "%b",            "101010",      INT_ARG, 42 );
    TEST_CBKPRINTF( "%3c",           "  a",         INT_ARG, 'a' );
    TEST_CBKPRINTF( "%3d",           "1234",        INT_ARG, 1234 );
    TEST_CBKPRINTF( "%3h",           "",            NO_ARG, 0 );
    TEST_CBKPRINTF( "%k%m%q%r%t%v%y%z", "",         NO_ARG, 0 );
    TEST_CBKPRINTF( "%-1d",          "2",              INT_ARG, 2 );
    TEST_CBKPRINTF( "%2.4f",         "8.6000",         DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "%0f",           "0.600000",       DOUBLE_ARG, 0.6 );
    TEST_CBKPRINTF( "%.0f",          "1",              DOUBLE_ARG, 0.6 );
    TEST_CBKPRINTF( "%2.4e",         "8.6000e+00",     DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "% 2.4e",        " 8.6000e+00",    DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "% 014.4e",      " 0008.6000e+00", DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "% 2.4e",        "-8.6000e+00",    DOUBLE_ARG,  -8.6 );
    TEST_CBKPRINTF( "%+2.4e",        "+8.6000e+00",    DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "%2.4g",         "8.6",            DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "%2.4Lf",        "8.6000",         LONG_DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "%0Lf",          "0.600000",       LONG_DOUBLE_ARG, 0.6 );
    TEST_CBKPRINTF( "%.0Lf",         "1",              LONG_DOUBLE_ARG, 0.6 );
    TEST_CBKPRINTF( "%2.4Le",        "8.6000e+00",     LONG_DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "% 2.4Le",       " 8.6000e+00",    LONG_DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "% 014.4Le",     " 0008.6000e+00", LONG_DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "% 2.4Le",       "-8.6000e+00",    LONG_DOUBLE_ARG,  -8.6 );
    TEST_CBKPRINTF( "%+2.4Le",       "+8.6000e+00",    LONG_DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "%2.4Lg",        "8.6",            LONG_DOUBLE_ARG, 8.6 );
    TEST_CBKPRINTF( "%-i",           "-1",             INT_ARG, -1 );
    TEST_CBKPRINTF( "%-i",           "1",              INT_ARG, 1 );
    TEST_CBKPRINTF( "%+i",           "+1",             INT_ARG, 1 );
    TEST_CBKPRINTF( "%o",            "12",             INT_ARG, 10 );
    TEST_CBKPRINTF( "%s",            "<NULL>",         PCHAR_ARG, NULL );
    TEST_CBKPRINTF( "%s",            "%%%%",           PCHAR_ARG, "%%%%" );
    TEST_CBKPRINTF( "%u",            "4294967295",     INT_ARG, -1 );
    TEST_CBKPRINTF( "%w",            "",               INT_ARG, -1 );
    TEST_CBKPRINTF( "%h",            "",               INT_ARG, -1 );
    TEST_CBKPRINTF( "%j",            "",               INTMAX_ARG, -1 );
    TEST_CBKPRINTF( "%jd",           "-1",             INTMAX_ARG, -1 );
    TEST_CBKPRINTF( "%F",            "0.000000",       DOUBLE_ARG, 0 );
    TEST_CBKPRINTF( "%N",            "",               INT_ARG, -1 );
    TEST_CBKPRINTF( "%H",            "",               INT_ARG, -1 );
    TEST_CBKPRINTF( "x%cx",          "xXx",            INT_ARG, 0x100+'X' );
    TEST_CBKPRINTF( "%%0",           "%0",             NO_ARG, 0 );
    TEST_CBKPRINTF( "%%%%%%%%",      "%%%%",           NO_ARG, 0 );
    TEST_CBKPRINTF( "%%%%%d%%%%",    "%%42%%",         INT_ARG, 42 );
    TEST_CBKPRINTF( "%hx",           "2345",           INT_ARG, 0x12345 );
    TEST_CBKPRINTF( "%hhx",          "23",             INT_ARG, 0x123 );
    TEST_CBKPRINTF( "%hhx",          "45",             INT_ARG, 0x12345 );
    TEST_CBKPRINTF( "%c",            "a",              INT_ARG, 'a' );
    TEST_CBKPRINTF( "%c",            "\x82",           INT_ARG, 0xa082 );
    TEST_CBKPRINTF( "%C",            "a",              INT_ARG, 'a' );
    TEST_CBKPRINTF( "%C",            "\xe3\x81\x82" /*""*/, INT_ARG, 0x3042 );
    TEST_CBKPRINTF( "%l4c",          "\xe3\x81\x82" /*""*/, INT_ARG, 0x3042 );
    TEST_CBKPRINTF( "%l2c",          "\xe3\x81\x82" /*""*/, INT_ARG, 0x3042 );
    TEST_CBKPRINTF( "a%Cb",          "a\xe3\x81\x82" "b",   INT_ARG, 0x3042 );
    TEST_CBKPRINTF( "%lld",          "-8589934591",   ULONGLONG_ARG, ((unsigned long long)0xffffffff)*0xffffffff );
    TEST_CBKPRINTF( "%I32d",         "1",             INT_ARG, 1 );
    TEST_CBKPRINTF( "%.0f",          "-2",            DOUBLE_ARG, -1.5 );
    TEST_CBKPRINTF( "%.0f",          "-1",            DOUBLE_ARG, -0.5 );
    TEST_CBKPRINTF( "%.0f",          "1",             DOUBLE_ARG, 0.5 );
    TEST_CBKPRINTF( "%.0f",          "2",             DOUBLE_ARG, 1.5 );

    TEST_CBKPRINTF( "%.3e", "0.000e+00",    DOUBLE_ARG, 0.0 );
    TEST_CBKPRINTF( "%.3e", "5.000e+00",    DOUBLE_ARG, 5e-0 );
    TEST_CBKPRINTF( "%.3e", "5.000e+00",    DOUBLE_ARG, 5e+0 );
    TEST_CBKPRINTF( "%.3e", "5.000e-01",    DOUBLE_ARG, 5e-1 );
    TEST_CBKPRINTF( "%.3e", "5.000e+01",    DOUBLE_ARG, 5e+1 );
    TEST_CBKPRINTF( "%.3e", "5.000e-02",    DOUBLE_ARG, 5e-2 );
    TEST_CBKPRINTF( "%.3e", "5.000e+02",    DOUBLE_ARG, 5e+2 );
    TEST_CBKPRINTF( "%.3e", "5.000e-03",    DOUBLE_ARG, 5e-3 );
    TEST_CBKPRINTF( "%.3e", "5.000e+03",    DOUBLE_ARG, 5e+3 );
    TEST_CBKPRINTF( "%.3e", "9.995e+00",    DOUBLE_ARG, 9.995e-0 );
    TEST_CBKPRINTF( "%.3e", "9.995e+00",    DOUBLE_ARG, 9.995e+0 );
    TEST_CBKPRINTF( "%.3e", "9.995e-01",    DOUBLE_ARG, 9.995e-1 );
    TEST_CBKPRINTF( "%.3e", "9.995e+01",    DOUBLE_ARG, 9.995e+1 );
    TEST_CBKPRINTF( "%.3e", "9.995e-02",    DOUBLE_ARG, 9.995e-2 );
    TEST_CBKPRINTF( "%.3e", "9.995e+02",    DOUBLE_ARG, 9.995e+2 );
    TEST_CBKPRINTF( "%.3e", "1.000e+00",    DOUBLE_ARG,  1.0 );
    TEST_CBKPRINTF( "%.3e", "-1.000e+00",   DOUBLE_ARG, -1.0 );
    TEST_CBKPRINTF( "%.3e", "1.000e+01",    DOUBLE_ARG,  1e+1 );
    TEST_CBKPRINTF( "%.3e", "-1.000e-01",   DOUBLE_ARG, -1e-1 );
    TEST_CBKPRINTF( "%.3e", "-1.000e+01",   DOUBLE_ARG, -1e+1 );
    TEST_CBKPRINTF( "%.3e", "1.000e-01",    DOUBLE_ARG,  1e-1 );
    TEST_CBKPRINTF( "%.3e", "-1.000e-01",   DOUBLE_ARG, -1e-1 );
    TEST_CBKPRINTF( "%.3e", "1.000e+02",    DOUBLE_ARG,  1e+2 );
    TEST_CBKPRINTF( "%.3e", "1.000e-02",    DOUBLE_ARG,  1e-2 );
    TEST_CBKPRINTF( "%.3e", "1.000e+03",    DOUBLE_ARG,  1e+3 );
    TEST_CBKPRINTF( "%.3e", "1.000e-03",    DOUBLE_ARG,  1e-3 );

    TEST_CBKPRINTF( "%.3Le", "0.000e+00",   LONG_DOUBLE_ARG, 0.0 );
    TEST_CBKPRINTF( "%.3Le", "5.000e+00",   LONG_DOUBLE_ARG, 5e-0 );
    TEST_CBKPRINTF( "%.3Le", "5.000e+00",   LONG_DOUBLE_ARG, 5e+0 );
    TEST_CBKPRINTF( "%.3Le", "5.000e-01",   LONG_DOUBLE_ARG, 5e-1 );
    TEST_CBKPRINTF( "%.3Le", "5.000e+01",   LONG_DOUBLE_ARG, 5e+1 );
    TEST_CBKPRINTF( "%.3Le", "5.000e-02",   LONG_DOUBLE_ARG, 5e-2 );
    TEST_CBKPRINTF( "%.3Le", "5.000e+02",   LONG_DOUBLE_ARG, 5e+2 );
    TEST_CBKPRINTF( "%.3Le", "5.000e-03",   LONG_DOUBLE_ARG, 5e-3 );
    TEST_CBKPRINTF( "%.3Le", "5.000e+03",   LONG_DOUBLE_ARG, 5e+3 );
    TEST_CBKPRINTF( "%.3Le", "9.995e+00",   LONG_DOUBLE_ARG, 9.995e-0 );
    TEST_CBKPRINTF( "%.3Le", "9.995e+00",   LONG_DOUBLE_ARG, 9.995e+0 );
    TEST_CBKPRINTF( "%.3Le", "9.995e-01",   LONG_DOUBLE_ARG, 9.995e-1 );
    TEST_CBKPRINTF( "%.3Le", "9.995e+01",   LONG_DOUBLE_ARG, 9.995e+1 );
    TEST_CBKPRINTF( "%.3Le", "9.995e-02",   LONG_DOUBLE_ARG, 9.995e-2 );
    TEST_CBKPRINTF( "%.3Le", "9.995e+02",   LONG_DOUBLE_ARG, 9.995e+2 );
    TEST_CBKPRINTF( "%.3Le", "1.000e+00",   LONG_DOUBLE_ARG,  1.0 );
    TEST_CBKPRINTF( "%.3Le", "-1.000e+00",  LONG_DOUBLE_ARG, -1.0 );
    TEST_CBKPRINTF( "%.3Le", "1.000e+01",   LONG_DOUBLE_ARG,  1e+1 );
    TEST_CBKPRINTF( "%.3Le", "-1.000e-01",  LONG_DOUBLE_ARG, -1e-1 );
    TEST_CBKPRINTF( "%.3Le", "-1.000e+01",  LONG_DOUBLE_ARG, -1e+1 );
    TEST_CBKPRINTF( "%.3Le", "1.000e-01",   LONG_DOUBLE_ARG,  1e-1 );
    TEST_CBKPRINTF( "%.3Le", "-1.000e-01",  LONG_DOUBLE_ARG, -1e-1 );
    TEST_CBKPRINTF( "%.3Le", "1.000e+02",   LONG_DOUBLE_ARG,  1e+2 );
    TEST_CBKPRINTF( "%.3Le", "1.000e-02",   LONG_DOUBLE_ARG,  1e-2 );
    TEST_CBKPRINTF( "%.3Le", "1.000e+03",   LONG_DOUBLE_ARG,  1e+3 );
    TEST_CBKPRINTF( "%.3Le", "1.000e-03",   LONG_DOUBLE_ARG,  1e-3 );

    TEST_CBKPRINTF( "%.3a", "-0x6.8dcp-3",  DOUBLE_ARG,  -1.6e-3 );
    TEST_CBKPRINTF( "%.3A", "-0X6.8DCP-3",  DOUBLE_ARG,  -1.6e-3 );
    TEST_CBKPRINTF( "%.3La", "-0x6.8dcp-3", LONG_DOUBLE_ARG,  -1.6e-3 );
    TEST_CBKPRINTF( "%.3LA", "-0X6.8DCP-3", LONG_DOUBLE_ARG,  -1.6e-3 );

    TEST_CBKPRINTF( "%5.2e%.0f",  "  inf3",   DOUBLE_ARG, *(float*) &inf ARG(3.0));
    TEST_CBKPRINTF( "%5.2e%.0f",  " -inf3",   DOUBLE_ARG, *(float*) &ninf ARG(3.0));
    TEST_CBKPRINTF( "%5.2e%.0f",  "  nan3",   DOUBLE_ARG, *(float*) &nan ARG(3.0));
    TEST_CBKPRINTF( "%5.2e%.0f",  " -nan3",   DOUBLE_ARG, *(float*) &nnan ARG(3.0));
    TEST_CBKPRINTF( "%5.2E%.0f",  "  INF-3",  DOUBLE_ARG, *(float*) &inf ARG(-3.0));
    TEST_CBKPRINTF( "%5.2E%.0f",  " -INF-3",  DOUBLE_ARG, *(float*) &ninf ARG(-3.0));
    TEST_CBKPRINTF( "%5.2E%.0f",  "  NAN-3",  DOUBLE_ARG, *(float*) &nan ARG(-3.0));
    TEST_CBKPRINTF( "%5.2E%.0f",  " -NAN-3",  DOUBLE_ARG, *(float*) &nnan ARG(-3.0));

    TEST_CBKPRINTF( "%5.2Le%.0Lf", "  inf3",  LONG_DOUBLE_ARG, *(float*) &inf ARG((long double) 3.0));
    TEST_CBKPRINTF( "%5.2Le%.0Lf", " -inf3",  LONG_DOUBLE_ARG, *(float*) &ninf ARG((long double) 3.0));
    TEST_CBKPRINTF( "%5.2Le%.0Lf", "  nan3",  LONG_DOUBLE_ARG, *(float*) &nan ARG((long double) 3.0));
    TEST_CBKPRINTF( "%5.2Le%.0Lf", " -nan3",  LONG_DOUBLE_ARG, *(float*) &nnan ARG((long double) 3.0));
    TEST_CBKPRINTF( "%5.2LE%.0Lf", "  INF-3", LONG_DOUBLE_ARG, *(float*) &inf ARG((long double) -3.0));
    TEST_CBKPRINTF( "%5.2LE%.0Lf", " -INF-3", LONG_DOUBLE_ARG, *(float*) &ninf ARG((long double) -3.0));
    TEST_CBKPRINTF( "%5.2LE%.0Lf", "  NAN-3", LONG_DOUBLE_ARG, *(float*) &nan ARG((long double) -3.0));
    TEST_CBKPRINTF( "%5.2LE%.0Lf", " -NAN-3", LONG_DOUBLE_ARG, *(float*) &nnan ARG((long double) -3.0));


    TEST_CBKPRINTF("%+5.10r2E %+10.10r2e %+10.10r7e %+10.10r4e %+10.0e %+10.0e ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
                    DOUBLE_ARG, 3.9 ARG(7.89456123e-307) ARG(7.89456123e+307) ARG(789456123.0) ARG(0.789456123) ARG(DBL_MIN));

    TEST_CBKPRINTF("%+5.10r2LE %+10.10r2Le %+10.10r7Le %+10.10r4Le %+10.0Le %+10.0Le ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
                    LONG_DOUBLE_ARG, 3.9 ARG((long double) 7.89456123e-307) ARG((long double) 7.89456123e+307) ARG((long double) 789456123.0) ARG((long double) 0.789456123) ARG((long double) DBL_MIN));

    return (bRet);
} /* test_ssprintf() */


/* -------------------------------------------------------------------------- *\
   main function
\* -------------------------------------------------------------------------- */
int main(int argc, char * argv[])
{
    int iRet = 1;
    if(!test_ssprintf())
         goto Exit;

    iRet = 0;

    Exit:;

    if(!iRet)
        printf("All tests passed!\n");
    else
        printf("Tests failed!\n");

    return (iRet);
}/* main() */


/* ========================================================================== *\
   END OF FILE
\* ========================================================================== */
