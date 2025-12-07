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
*  COPYRIGHT:     (c) 2024 Dipl.-Ing. Klaus Lux (Aachen, Germany)             *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  REPOSITORY:     https://github/klux21/callback_printf                      *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
* Civil Usage Public License, Version 1.1, January 2024                       *
*                                                                             *
* Redistribution and use in source and binary forms, with or without          *
* modification, are permitted provided that the following conditions are met: *
*                                                                             *
* 1. Redistributions of source code must retain the above copyright           *
*    notice, this list of conditions, the explanation of terms                *
*    and the following disclaimer.                                            *
*                                                                             *
* 2. Redistributions in binary form must reproduce the above copyright        *
*    notice, this list of conditions and the following disclaimer in the      *
*    documentation or other materials provided with the distribution.         *
*                                                                             *
* 3. All modified files must carry prominent notices stating that the         *
*    files have been changed.                                                 *
*                                                                             *
* 4. The source code and binary forms and any derivative works are not        *
*    stored or executed in systems or devices which are designed or           *
*    intended to harm, to kill or to forcibly immobilize people.              *
*                                                                             *
* 5. The source code and binary forms and any derivative works are not        *
*    stored or executed in systems or devices which are intended to           *
*    monitor, to track, to change or to control the behavior, the             *
*    constitution, the location or the communication of any people or         *
*    their property without the explicit and prior agreement of those         *
*    people except those devices and systems are solely designed for          *
*    saving or protecting peoples life or health.                             *
*                                                                             *
* 6. The source code and binary forms and any derivative works are not        *
*    stored or executed in any systems or devices that are intended           *
*    for the production of any of the systems or devices that                 *
*    have been stated before except the ones for saving or protecting         *
*    peoples life or health only.                                             *
*                                                                             *
* The term 'systems' in all clauses shall include all types and combinations  *
* of physical, virtualized or simulated hardware and software and any kind    *
* of data storage.                                                            *
*                                                                             *
* The term 'devices' shall include any kind of local or non-local control     *
* system of the stated devices as part of that device als well. Any assembly  *
* of more than one device is one and the same device regarding this license.  *
*                                                                             *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  *
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     *
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  *
* POSSIBILITY OF SUCH DAMAGE.                                                 *
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

int test_svsprintf(int line, const char * pout, const char * call, const char * pfmt, ...)
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
       printf("test_callback_printf.c:%d : svsprintf output length %zd (%s) for '%s' does not match retval %zd of svsnprintf!\n", line, sRet, buf, call, snRet);
       goto Exit;
    }

    if(buf2[17] != '\xfe')
    {
       buf2[24] = '\0';
       printf("test_callback_printf.c:%d : svsnprintf returned %zd (%s) for '%s' but touched data behind the provided buffer!\n", line, snRet, buf2, call);
       goto Exit;
    }

    if(buf[sRet+1] != '\xfe')
    {
       buf2[24] = '\0';
       printf("test_callback_printf.c:%d : svsprintf returned %zd (%s) for '%s' but touched data behind the string end!\n", line, sRet, buf, call);
       goto Exit;
    }

    if(strncmp(buf, buf2, 17))
    {
       printf("test_callback_printf.c:%d : written data of svsprintf (%zd bytes: '%.17s') and svsnprintf (%zd bytes: '%.17s')) differ for '%s'!\n", line, sRet, buf, snRet, buf2, call);
       goto Exit;
    }

#if DEBUG
    va_start(VarArgs, pfmt);
    vsprintf(buf2, pfmt, VarArgs); /* for comparison of data with vsprintf in a debugger */
    va_end(VarArgs);
#endif

    if(sRet != strlen(buf))
    {
       printf("test_callback_printf.c:%d : output length %zd (%s) for '%s' does not match retval %zd !\n", line, strlen(buf), buf, call, sRet);
       goto Exit;
    }

    if(strcmp(buf, pout))
    {
       printf("test_callback_printf.c:%d : output '%s' to '%s' doesn't match expected '%s' !\n", line, buf, call, pout);
       goto Exit;
    }

    bRet = 1;
    Exit:;
    return (bRet);
} /* int test_svsprintf(...) */

/* ------------------------------------------------------------------------- *\
   Write function for our own %V argument data that is just the output of
   some string data in a PRINTF_V_DATA struct here. 
\* ------------------------------------------------------------------------- */

size_t cbfunc (void *            pUserData,
               PRINTF_CALLBACK * pCB,
               void *            pvdata,
               size_t            precision,
               size_t            minimum_width,
               uint8_t           left_justified,
               uint8_t           prefixing)
{
   char   buffer[100];
   PRINTF_V_DATA * pvd = (PRINTF_V_DATA *) pvdata;

   size_t length = ssnprintf(buffer, sizeof(buffer), "%.*s", (int) (precision <= pvd->size ? precision : pvd->size), (char *) pvd->pdata);

   return ( cbk_print_string( pUserData, pCB, buffer, length, minimum_width, left_justified));
} /* size_t cbfunc(...) */

int test_ssprintf()
{
   int bRet = 1;

   uint8_t  s8[]  = { 'H','e','l','l','o',' ','w','o','r','l','d','!',' ',0xc4,0xd6,0xdc,0xdf,0xe4,0xf6,0xfc,0}; 
   uint16_t s16[] = { 'H','e','l','l','o',' ','w','o','r','l','d','!',' ',0xc4,0xd6,0xdc,0xdf,0xe4,0xf6,0xfc,0}; 
   uint32_t s32[] = { 'H','e','l','l','o',' ','w','o','r','l','d','!',' ',0xc4,0xd6,0xdc,0xdf,0xe4,0xf6,0xfc,0}; 

   PRINTF_V_DATA cbdata = { &cbfunc, 12, (void *) "Hello world!"}; /* output data for %v and %V samples */

   uint32_t u_inf  = 0x7f800000;
   uint32_t u_ninf = 0xff800000;
   uint32_t u_nan  = 0x7fc00000;
   uint32_t u_nnan = 0xffc00000;

   float inf;
   float ninf;
   float nan;
   float nnan;

   memcpy(&inf,  &u_inf,  4);
   memcpy(&ninf, &u_ninf, 4);
   memcpy(&nan,  &u_nan,  4);
   memcpy(&nnan, &u_nnan, 4);

#define ARG(x)          ,x

#define TEST_VSPRINTF(pfmt, pout, arg) bRet &= test_svsprintf(__LINE__, pout, #pfmt "," #arg, pfmt, arg);

    TEST_VSPRINTF( "Hallo Welt!", "Hallo Welt!",  0);
    TEST_VSPRINTF( "%.10s!",      "Hallo Welt!",  "Hallo Welt");
    TEST_VSPRINTF( "%.*s!",       "Hallo Welt!", (int) 10 ARG("Hallo Welt"));
    TEST_VSPRINTF( "%.5s %.5s",   "Hallo Welt!",  "Hallo" ARG("Welt!"));
    TEST_VSPRINTF( "%-6.5s%4.5s!","Hallo Welt!",  "Hallo" ARG("Welt"));
    TEST_VSPRINTF( "%c%c%c%c%c %c%c%c%c!", "Hallo Welt!", 'H' ARG('a') ARG('l') ARG('l') ARG('o') ARG('W') ARG('e') ARG('l') ARG('t'));
    TEST_VSPRINTF( "%02d/%02d/%04d %02d:%02d:%02d", "01/01/1970 23:59:59", 1 ARG(1) ARG(1970) ARG(23) ARG(59) ARG(59));
    TEST_VSPRINTF( "%.2d/%.2d/%.4d %.2d:%.2d:%.2d", "01/01/1970 23:59:59", 1 ARG(1) ARG(1970) ARG(23) ARG(59) ARG(59));
    TEST_VSPRINTF( "%+#23.15e",   " +1.000000000000000e-01", (double) 1.0e-1 );
    TEST_VSPRINTF( "%+#23.15e",   " +3.900000000000000e+00", (double) 3.9 );
    TEST_VSPRINTF( "%+#23.14e",   " +7.89456123000000e-307", (double) 7.89456123e-307 );
    TEST_VSPRINTF( "%+#23.14e",   " +7.89456123000000e+307", (double) 7.89456123e+307 );
    TEST_VSPRINTF( "%+#23.15e",   " +7.894561230000000e+08", (double) 789456123.0 );
    TEST_VSPRINTF( "%-#23.15e",   "7.894561230000000e+08  ", (double) 789456123.0 );
    TEST_VSPRINTF( "%#23.15e",    "  7.894561230000000e+08", (double)789456123.0 );
    TEST_VSPRINTF( "%#1.1g",      "8.e+08", (double) 789456123.0 );
    TEST_VSPRINTF( "%+#23.15Le",  " +1.000000000000000e-01", (long double) 1.0e-1l );
    TEST_VSPRINTF( "%+#23.15Le",  " +3.900000000000000e+00", (long double) 3.9l );
    TEST_VSPRINTF( "%+#27.6e",    "             +1.594561e-317", (double) 1.59456123e-317 );
    TEST_VSPRINTF( "%+#27.16e",    "   +1.5945612300000000e+308", (double) 1.59456123e+308 );
#ifdef _WIN32
    TEST_VSPRINTF( "%+#27.15Le",    "   +9.89456123000000000e-307",(long double) 9.89456123e-307l );
    TEST_VSPRINTF( "%+#27.15Le",    "   +9.89456123000000000e+307",(long double) 9.89456123e+307l );
#else
    TEST_VSPRINTF( "%+#24.14Le",  " +7.89456123000000e-4307", (long double) 7.89456123e-4307l );
    TEST_VSPRINTF( "%+#24.14Le",  " +7.89456123000000e+4307", (long double) 7.89456123e+4307l );
#endif
    TEST_VSPRINTF( "%+#23.15Le",  " +7.894561230000000e+08", (long double) 789456123.0l );
    TEST_VSPRINTF( "%-#23.15Le",  "7.894561230000000e+08  ", (long double) 789456123.0l );
    TEST_VSPRINTF( "%#23.15Le",   "  7.894561230000000e+08", (long double) 789456123.0l );
    TEST_VSPRINTF( "%#1.1Lg",     "8.e+08",                  (long double) 789456123.0l );
    TEST_VSPRINTF( "%I64d",       "-8589934591",             (unsigned long long) ((unsigned long long)0xffffffff)*0xffffffff );
    TEST_VSPRINTF( "%+8I64d",     "    +100",                (unsigned long long) 100 );
    TEST_VSPRINTF( "%+.8I64d",    "+00000100",               (unsigned long long) 100 );
    TEST_VSPRINTF( "%+10.8I64d",  " +00000100",              (unsigned long long) 100 );
    TEST_VSPRINTF( "%_1I64d",     "",                        (unsigned long long) 100 );
    TEST_VSPRINTF( "%_1lld",     "",                         (unsigned long long) 100 );
    TEST_VSPRINTF( "%-1.5I64d",   "-00100",                  (unsigned long long) -100 );
    TEST_VSPRINTF( "%5I64d",      "  100",                   (unsigned long long) 100 );
    TEST_VSPRINTF( "%5I64d",      " -100",                   (unsigned long long) -100 );
    TEST_VSPRINTF( "%-5I64d",     "100  ",                   (unsigned long long) 100 );
    TEST_VSPRINTF( "%-5I64d",     "-100 ",                   (unsigned long long) -100 );
    TEST_VSPRINTF( "%-.5I64d",    "00100",                   (unsigned long long) 100 );
    TEST_VSPRINTF( "%-.5I64d",    "-00100",                  (unsigned long long) -100 );
    TEST_VSPRINTF( "%-8.5I64d",   "00100   ",                (unsigned long long) 100 );
    TEST_VSPRINTF( "%-8.5I64d",   "-00100  ",                (unsigned long long) -100 );
    TEST_VSPRINTF( "%05I64d",     "00100",                   (unsigned long long) 100 );
    TEST_VSPRINTF( "%05I64d",     "-0100",                   (unsigned long long) -100 );
    TEST_VSPRINTF( "% I64d",      " 100",                    (unsigned long long) 100 );
    TEST_VSPRINTF( "% I64d",      "-100",                    (unsigned long long) -100 );
    TEST_VSPRINTF( "% 5I64d",     "  100",                   (unsigned long long)  100 );
    TEST_VSPRINTF( "% 5I64d",     " -100",                   (unsigned long long)  -100 );
    TEST_VSPRINTF( "% .5I64d",    " 00100",                  (unsigned long long) 100 );
    TEST_VSPRINTF( "% .5I64d",    "-00100",                  (unsigned long long) -100 );
    TEST_VSPRINTF( "% 8.5I64d",   "   00100",                (unsigned long long) 100 );
    TEST_VSPRINTF( "% 8.5I64d",   "  -00100",                (unsigned long long) -100 );
    TEST_VSPRINTF( "%.0I64d",     "",                        (unsigned long long) 0 );
    TEST_VSPRINTF( "%.0lld",        "",                          (unsigned long long) 0 );
    TEST_VSPRINTF( "%8.0lld",       "        ",                  (unsigned long long) 0 );
    TEST_VSPRINTF( "%08.0lld",      "        ",                  (unsigned long long) 0 );
    TEST_VSPRINTF( "%#+21.18I64x",  " 0x00ffffffffffffff9c",     (unsigned long long) -100 );
    TEST_VSPRINTF( "%#.25I64o",     "0001777777777777777777634", (unsigned long long) -100 );
    TEST_VSPRINTF( "%#+24.20I64o",  " 01777777777777777777634",  (unsigned long long) -100 );
    TEST_VSPRINTF( "%#+18.21I64X",  "0X00000FFFFFFFFFFFFFF9C",   (unsigned long long) -100 );
    TEST_VSPRINTF( "%#+20.24I64o",  "001777777777777777777634",  (unsigned long long) -100 );
    TEST_VSPRINTF( "%#+25.22I64u",  "   0018446744073709551615", (unsigned long long) -1 );
    TEST_VSPRINTF( "%#+25.22I64u",  "   0018446744073709551615", (unsigned long long) -1 );
    TEST_VSPRINTF( "%#+30.25I64u",  "     0000018446744073709551615", (unsigned long long) -1 );
    TEST_VSPRINTF( "%+#25.22I64d",  "  -0000000000000000000001", (unsigned long long) -1 );
    TEST_VSPRINTF( "%#-8.5I64o",    "00144   ",                  (unsigned long long) 100 );
    TEST_VSPRINTF( "%#-+ 08.5I64d", "+00100  ",                  (unsigned long long) 100 );
    TEST_VSPRINTF( "%.80I64d",      "00000000000000000000000000000000000000000000000000000000000000000000000000000001",  (unsigned long long) 1 );
    TEST_VSPRINTF( "% .80I64d",     " 00000000000000000000000000000000000000000000000000000000000000000000000000000001", (unsigned long long) 1 );
    TEST_VSPRINTF( "% .80d",        " 00000000000000000000000000000000000000000000000000000000000000000000000000000001", (int) 1 );
    TEST_VSPRINTF( "%I",            "" , (int) 1 );
    TEST_VSPRINTF( "%Iq",           "" , (int) 1 );
    TEST_VSPRINTF( "%Ihd",          "" , (int) 1 );
    TEST_VSPRINTF( "%I0d",          "" , (int) 1 );
    TEST_VSPRINTF( "%I64D",         "" , (unsigned long long) -1 );
    TEST_VSPRINTF( "%zx",           "1",   (size_t) 1 );
    TEST_VSPRINTF( "%zx",           sizeof(size_t) > 4 ? "ffffffffffffffff" : "ffffffff", (size_t) -1 );
    TEST_VSPRINTF( "%z",            "" ,   (size_t) 1 );
    TEST_VSPRINTF( "%tx",           "1",   (ptrdiff_t) 1 );
    TEST_VSPRINTF( "%tx",           sizeof(size_t) > 4 ? "8000000000000000" : "80000000", (ptrdiff_t) 1 << (sizeof(ptrdiff_t) * 8 - 1));
    TEST_VSPRINTF( "%t",            "" ,   (ptrdiff_t) 1 );
    TEST_VSPRINTF( "% d",           " 1",  (int) 1 );
    TEST_VSPRINTF( "%+ d",          "+1",  (int) 1 );
    TEST_VSPRINTF( "%S",            "wide", (wchar_t *) L"wide" );
    TEST_VSPRINTF( "%S",            "wide \xc3\x84\xc3\x96\xc3\x9c\xc3\x9f\xc3\xa4\xc3\xb6\xc3\xbc", (wchar_t *) L"wide \xc4\xd6\xdc\xdf\xe4\xf6\xfc" );
    TEST_VSPRINTF( "%ls",           "wide \xc3\x84\xc3\x96\xc3\x9c\xc3\x9f\xc3\xa4\xc3\xb6\xc3\xbc", (wchar_t *) L"wide \xc4\xd6\xdc\xdf\xe4\xf6\xfc" );
    TEST_VSPRINTF( "%l1s",          "Hello world! \xc3\x84\xc3\x96\xc3\x9c\xc3\x9f\xc3\xa4\xc3\xb6\xc3\xbc", s8 );
    TEST_VSPRINTF( "%l2s",          "Hello world! \xc3\x84\xc3\x96\xc3\x9c\xc3\x9f\xc3\xa4\xc3\xb6\xc3\xbc", s16 );
    TEST_VSPRINTF( "%l4s",          "Hello world! \xc3\x84\xc3\x96\xc3\x9c\xc3\x9f\xc3\xa4\xc3\xb6\xc3\xbc", s32 );
    TEST_VSPRINTF( "%-04c",         "1   ", (int) '1' );
    TEST_VSPRINTF( "%#012x",        "0x0000000001", (int) 1 );
    TEST_VSPRINTF( "%#012x",        "000000000000", (int) 0 );
    TEST_VSPRINTF( "%#04.8x",       "0x00000001",   (int) 1 );
    TEST_VSPRINTF( "%#04.8x",       "00000000",     (int) 0 );
    TEST_VSPRINTF( "%#-08.2x",      "0x01    ",     (int) 1 );
    TEST_VSPRINTF( "%#-08.2x",      "00      ",     (int) 0 );
    TEST_VSPRINTF( "%#.0x",         "0x1",          (int) 1 );
    TEST_VSPRINTF( "%#.0x",         "",            (int) 0 );
    TEST_VSPRINTF( "%#08o",         "00000001",    (int) 1 );
    TEST_VSPRINTF( "%#o",           "01",          (int) 1 );
    TEST_VSPRINTF( "%#o",           "0",           (int) 0 );
    TEST_VSPRINTF( "%04s",          " foo",        (char *) "foo" );
    TEST_VSPRINTF( "%.1s",          "f",           (char *) "foo" );
    TEST_VSPRINTF( "%.0s",          "",            (char *) "foo" );
    TEST_VSPRINTF( "hello",         "hello",        0 );
    TEST_VSPRINTF( "%ws",           "",            (wchar_t *) L"wide" );
    TEST_VSPRINTF( "%-10ws",        "",            (wchar_t *) L"wide" );
    TEST_VSPRINTF( "%10ws",         "",            (wchar_t *) L"wide" );
    TEST_VSPRINTF( "%#+ -03whlls",  "",            (wchar_t *) L"wide" );
    TEST_VSPRINTF( "%w0s",          "",            (wchar_t *) L"wide" );
    TEST_VSPRINTF( "%w-s",          "",            (wchar_t *) L"wide" );
    TEST_VSPRINTF( "%ls",           "wide",        (wchar_t *) L"wide" );
    TEST_VSPRINTF( "%Ls",           "",            (wchar_t *) "not wide" );
    TEST_VSPRINTF( "%6.4c",         "  ____",      (int) '_' );
    TEST_VSPRINTF( "%-6.4c",         "____  ",     (int) '_' );
    TEST_VSPRINTF( "%b",            "101010",      (int) 42 );
    TEST_VSPRINTF( "%#b",           "0b101010",    (int) 42 );
    TEST_VSPRINTF( "%#B",           "0B101010",    (int) 42 );
    TEST_VSPRINTF( "%3c",           "  a",         (int) 'a' );
    TEST_VSPRINTF( "%-3c",          "a  ",         (int) 'a' );
    TEST_VSPRINTF( "%3d",           "1234",        (int) 1234 );
    TEST_VSPRINTF( "%3h",           "",             0 );
    TEST_VSPRINTF( "%k%m%q%r%t%v%y%z", "",          0 );
    TEST_VSPRINTF( "%-1d",          "2",              (int) 2 );
    TEST_VSPRINTF( "%2.4f",         "8.6000",         (double) 8.6 );
    TEST_VSPRINTF( "%0f",           "0.600000",       (double) 0.6 );
    TEST_VSPRINTF( "%.0f",          "1",              (double) 0.6 );
    TEST_VSPRINTF( "%2.4e",         "8.6000e+00",     (double) 8.6 );
    TEST_VSPRINTF( "% 2.4e",        " 8.6000e+00",    (double) 8.6 );
    TEST_VSPRINTF( "% 014.4e",      " 0008.6000e+00", (double) 8.6 );
    TEST_VSPRINTF( "% 2.4e",        "-8.6000e+00",    (double)  -8.6 );
    TEST_VSPRINTF( "%+2.4e",        "+8.6000e+00",    (double) 8.6 );
    TEST_VSPRINTF( "%2.4g",         "8.6",            (double) 8.6 );
    TEST_VSPRINTF( "%2.4Lf",        "8.6000",         (long double) 8.6 );
    TEST_VSPRINTF( "%0Lf",          "0.600000",       (long double) 0.6 );
    TEST_VSPRINTF( "%.0Lf",         "1",              (long double) 0.6 );
    TEST_VSPRINTF( "%2.4Le",        "8.6000e+00",     (long double) 8.6 );
    TEST_VSPRINTF( "% 2.4Le",       " 8.6000e+00",    (long double) 8.6 );
    TEST_VSPRINTF( "% 014.4Le",     " 0008.6000e+00", (long double) 8.6 );
    TEST_VSPRINTF( "% 2.4Le",       "-8.6000e+00",    (long double)  -8.6 );
    TEST_VSPRINTF( "%+2.4Le",       "+8.6000e+00",    (long double) 8.6 );
    TEST_VSPRINTF( "%2.4Lg",        "8.6",            (long double) 8.6 );
    TEST_VSPRINTF( "%-i",           "-1",             (int) -1 );
    TEST_VSPRINTF( "%-i",           "1",              (int) 1 );
    TEST_VSPRINTF( "%+i",           "+1",             (int) 1 );
    TEST_VSPRINTF( "%o",            "12",             (int) 10 );
    TEST_VSPRINTF( "%s",            "<NULL>",         (char *) NULL );
    TEST_VSPRINTF( "%s",            "%%%%",           (char *) "%%%%" );
    TEST_VSPRINTF( "%u",            "4294967295",     (int) -1 );
    TEST_VSPRINTF( "%u",            "2147483648",     (unsigned int) 0x80000000 );
    TEST_VSPRINTF( "%d",            "-2147483648",    (int) 0x80000000 );
    TEST_VSPRINTF( "%w",            "",               (int) -1 );
    TEST_VSPRINTF( "%h",            "",               (int) -1 );
    TEST_VSPRINTF( "%j",            "",               (intmax_t) -1 );
    TEST_VSPRINTF( "%jd",           "-1",             (intmax_t) -1 );
    TEST_VSPRINTF( "%F",            "0.000000",       (double) 0 );
    TEST_VSPRINTF( "%N",            "",               (int) -1 );
    TEST_VSPRINTF( "%H",            "",               (int) -1 );
    TEST_VSPRINTF( "x%cx",          "xXx",            (int) 0x100+'X' );
    TEST_VSPRINTF( "%%0",           "%0",              0 );
    TEST_VSPRINTF( "%%%%%%%%",      "%%%%",            0 );
    TEST_VSPRINTF( "%%%%%d%%%%",    "%%42%%",         (int) 42 );
    TEST_VSPRINTF( "%hx",           "2345",           (int) 0x12345 );
    TEST_VSPRINTF( "%hhx",          "23",             (int) 0x123 );
    TEST_VSPRINTF( "%hhx",          "45",             (int) 0x12345 );
    TEST_VSPRINTF( "%c",            "a",              (int) 'a' );
    TEST_VSPRINTF( "%c",            "\x82",           (int) 0xa082 );
    TEST_VSPRINTF( "%C",            "a",              (int) 'a' );
    TEST_VSPRINTF( "%C",            "\xe3\x81\x82" /*""*/, (int) 0x3042 );
    TEST_VSPRINTF( "%l4c",          "\xe3\x81\x82" /*""*/, (int) 0x3042 );
    TEST_VSPRINTF( "%l2c",          "\xe3\x81\x82" /*""*/, (int) 0x3042 );
    TEST_VSPRINTF( "a%Cb",          "a\xe3\x81\x82" "b",   (int) 0x3042 );
    TEST_VSPRINTF( "%lld",          "-8589934591",   (unsigned long long) ((unsigned long long)0xffffffff)*0xffffffff );
    TEST_VSPRINTF( "%llu",          "18446744065119617025", (unsigned long long) ((unsigned long long)0xffffffff)*0xffffffff );
    TEST_VSPRINTF( "%I32d",         "1",             (int) 1 );
    TEST_VSPRINTF( "%.0f",          "-2",            (double) -1.5 );
    TEST_VSPRINTF( "%.0f",          "-1",            (double) -0.5 );
    TEST_VSPRINTF( "%.0f",          "1",             (double) 0.5 );
    TEST_VSPRINTF( "%.0f",          "2",             (double) 1.5 );

    TEST_VSPRINTF( "%.3e", "0.000e+00",    (double) 0.0 );
    TEST_VSPRINTF( "%.3e", "5.000e+00",    (double) 5e-0 );
    TEST_VSPRINTF( "%.3e", "5.000e+00",    (double) 5e+0 );
    TEST_VSPRINTF( "%.3e", "5.000e-01",    (double) 5e-1 );
    TEST_VSPRINTF( "%.3e", "5.000e+01",    (double) 5e+1 );
    TEST_VSPRINTF( "%.3e", "5.000e-02",    (double) 5e-2 );
    TEST_VSPRINTF( "%.3e", "5.000e+02",    (double) 5e+2 );
    TEST_VSPRINTF( "%.3e", "5.000e-03",    (double) 5e-3 );
    TEST_VSPRINTF( "%.3e", "5.000e+03",    (double) 5e+3 );
    TEST_VSPRINTF( "%.3e", "9.995e+00",    (double) 9.995e-0 );
    TEST_VSPRINTF( "%.3e", "9.995e+00",    (double) 9.995e+0 );
    TEST_VSPRINTF( "%.3e", "9.995e-01",    (double) 9.995e-1 );
    TEST_VSPRINTF( "%.3e", "9.995e+01",    (double) 9.995e+1 );
    TEST_VSPRINTF( "%.3e", "9.995e-02",    (double) 9.995e-2 );
    TEST_VSPRINTF( "%.3e", "9.995e+02",    (double) 9.995e+2 );
    TEST_VSPRINTF( "%.2e", "1.00e+03",     (double) 9.9951e+2 );
    TEST_VSPRINTF( "%.2e", "-1.00e+03",    (double) -9.9951e+2 );
    TEST_VSPRINTF( "%.2e", "1.00e-01",     (double) 9.9951e-2 );
    TEST_VSPRINTF( "%.2e", "-1.00e-01",    (double) -9.9951e-2 );
    TEST_VSPRINTF( "%#.0e", "1.e+03",      (double) 9.51e+2 );
    TEST_VSPRINTF( "%#.0e", "-1.e+03",     (double) -9.51e+2 );
    TEST_VSPRINTF( "%#.0e", "1.e-01",      (double) 9.51e-2 );
    TEST_VSPRINTF( "%#.0e", "-1.e-01",     (double) -9.51e-2 );
    TEST_VSPRINTF( "%.3e", "1.000e+00",    (double)  1.0 );
    TEST_VSPRINTF( "%.3e", "-1.000e+00",   (double) -1.0 );
    TEST_VSPRINTF( "%.3e", "1.000e+01",    (double)  1e+1 );
    TEST_VSPRINTF( "%.3e", "-1.000e-01",   (double) -1e-1 );
    TEST_VSPRINTF( "%.3e", "-1.000e+01",   (double) -1e+1 );
    TEST_VSPRINTF( "%.3e", "1.000e-01",    (double)  1e-1 );
    TEST_VSPRINTF( "%.3e", "-1.000e-01",   (double) -1e-1 );
    TEST_VSPRINTF( "%.3e", "1.000e+02",    (double)  1e+2 );
    TEST_VSPRINTF( "%.3e", "1.000e-02",    (double)  1e-2 );
    TEST_VSPRINTF( "%.3e", "1.000e+03",    (double)  1e+3 );
    TEST_VSPRINTF( "%.3e", "1.000e-03",    (double)  1e-3 );

    TEST_VSPRINTF( "%.3Le", "0.000e+00",   (long double) 0.0 );
    TEST_VSPRINTF( "%.3Le", "5.000e+00",   (long double) 5e-0 );
    TEST_VSPRINTF( "%.3Le", "5.000e+00",   (long double) 5e+0 );
    TEST_VSPRINTF( "%.3Le", "5.000e-01",   (long double) 5e-1 );
    TEST_VSPRINTF( "%.3Le", "5.000e+01",   (long double) 5e+1 );
    TEST_VSPRINTF( "%.3Le", "5.000e-02",   (long double) 5e-2 );
    TEST_VSPRINTF( "%.3Le", "5.000e+02",   (long double) 5e+2 );
    TEST_VSPRINTF( "%.3Le", "5.000e-03",   (long double) 5e-3 );
    TEST_VSPRINTF( "%.3Le", "5.000e+03",   (long double) 5e+3 );
    TEST_VSPRINTF( "%.3Le", "9.995e+00",   (long double) 9.995e-0 );
    TEST_VSPRINTF( "%.3Le", "9.995e+00",   (long double) 9.995e+0 );
    TEST_VSPRINTF( "%.3Le", "9.995e-01",   (long double) 9.995e-1 );
    TEST_VSPRINTF( "%.3Le", "9.995e+01",   (long double) 9.995e+1 );
    TEST_VSPRINTF( "%.3Le", "9.995e-02",   (long double) 9.995e-2 );
    TEST_VSPRINTF( "%.3Le", "9.995e+02",   (long double) 9.995e+2 );
    TEST_VSPRINTF( "%#.0Le", "1.e+03",     (long double) 9.51e+2 );
    TEST_VSPRINTF( "%#.0Le", "-1.e+03",    (long double) -9.51e+2 );
    TEST_VSPRINTF( "%#.0Le", "1.e-01",     (long double) 9.51e-2 );
    TEST_VSPRINTF( "%#.0Le", "-1.e-01",    (long double) -9.51e-2 );
    TEST_VSPRINTF( "%.3Le", "1.000e+00",   (long double)  1.0 );
    TEST_VSPRINTF( "%.3Le", "-1.000e+00",  (long double) -1.0 );
    TEST_VSPRINTF( "%.3Le", "1.000e+01",   (long double)  1e+1 );
    TEST_VSPRINTF( "%.3Le", "-1.000e-01",  (long double) -1e-1 );
    TEST_VSPRINTF( "%.3Le", "-1.000e+01",  (long double) -1e+1 );
    TEST_VSPRINTF( "%.3Le", "1.000e-01",   (long double)  1e-1 );
    TEST_VSPRINTF( "%.3Le", "-1.000e-01",  (long double) -1e-1 );
    TEST_VSPRINTF( "%.3Le", "1.000e+02",   (long double)  1e+2 );
    TEST_VSPRINTF( "%.3Le", "1.000e-02",   (long double)  1e-2 );
    TEST_VSPRINTF( "%.3Le", "1.000e+03",   (long double)  1e+3 );
    TEST_VSPRINTF( "%.3Le", "1.000e-03",   (long double)  1e-3 );

    TEST_VSPRINTF( "%.3a",  "-0x1.a37p-10",  (double)       -1.6e-3 );
    TEST_VSPRINTF( "%.3A",  "-0X1.A37P-10",  (double)       -1.6e-3 );
    TEST_VSPRINTF( "%.3La", "-0x1.a37p-10",  (long double)  -1.6e-3 );
    TEST_VSPRINTF( "%.3LA", "-0X1.A37P-10",  (long double)  -1.6e-3 );

    TEST_VSPRINTF( "%5.2e%.0f",  "  inf3",   (double) inf ARG(3.0));
    TEST_VSPRINTF( "%5.2e%.0f",  " -inf3",   (double) ninf ARG(3.0));
    TEST_VSPRINTF( "%5.2e%.0f",  "  nan3",   (double) nan ARG(3.0));
    TEST_VSPRINTF( "%5.2e%.0f",  " -nan3",   (double) nnan ARG(3.0));
    TEST_VSPRINTF( "%5.2E%.0f",  "  INF-3",  (double) inf ARG(-3.0));
    TEST_VSPRINTF( "%5.2E%.0f",  " -INF-3",  (double) ninf ARG(-3.0));
    TEST_VSPRINTF( "%5.2E%.0f",  "  NAN-3",  (double) nan ARG(-3.0));
    TEST_VSPRINTF( "%5.2E%.0f",  " -NAN-3",  (double) nnan ARG(-3.0));

    TEST_VSPRINTF( "%5.2Le%.0Lf", "  inf3",  (long double) inf ARG((long double) 3.0));
    TEST_VSPRINTF( "%5.2Le%.0Lf", " -inf3",  (long double) ninf ARG((long double) 3.0));
    TEST_VSPRINTF( "%5.2Le%.0Lf", "  nan3",  (long double) nan ARG((long double) 3.0));
    TEST_VSPRINTF( "%5.2Le%.0Lf", " -nan3",  (long double) nnan ARG((long double) 3.0));
    TEST_VSPRINTF( "%5.2LE%.0Lf", "  INF-3", (long double) inf ARG((long double) -3.0));
    TEST_VSPRINTF( "%5.2LE%.0Lf", " -INF-3", (long double) ninf ARG((long double) -3.0));
    TEST_VSPRINTF( "%5.2LE%.0Lf", "  NAN-3", (long double) nan ARG((long double) -3.0));
    TEST_VSPRINTF( "%5.2LE%.0Lf", " -NAN-3", (long double) nnan ARG((long double) -3.0));

    TEST_VSPRINTF( "%# 01.1g",    " 1.e+01", (double) 9.8);
    TEST_VSPRINTF( "%# 01.1Lg",   " 1.e+01", (long double) 9.8l);
    TEST_VSPRINTF( "%#.1g",       "-4.e+04", (double) -40661.5);
    TEST_VSPRINTF( "%#.1Lg",      "-4.e+04", (long double) -40661.5l);
    TEST_VSPRINTF( "%#g",         "0.00000", (double) 0.0);
    TEST_VSPRINTF( "%#Lg",        "0.00000", (long double) 0.0l);
    TEST_VSPRINTF( "%g",          "0",       (double) 0.0);
    TEST_VSPRINTF( "%Lg",         "0",       (long double) 0.0l);
    TEST_VSPRINTF( "%g",          "490000",  (double) 490000.0l);
    TEST_VSPRINTF( "%Lg",         "490000",  (long double) 490000.0l);
    TEST_VSPRINTF( "%g",          "4.9e+06", (double) 4900000.0l);
    TEST_VSPRINTF( "%Lg",         "4.9e+06", (long double) 4900000.0l);
    TEST_VSPRINTF( "%.7g",        "4900000", (double) 4900000.0l);
    TEST_VSPRINTF( "%.7Lg",       "4900000", (long double) 4900000.0l);
    TEST_VSPRINTF( "%.7g",        "4.9e+07", (double) 49000000.0l);
    TEST_VSPRINTF( "%.7Lg",       "4.9e+07", (long double) 49000000.0l);
    TEST_VSPRINTF( "%.7G",        "4.9E+07", (double) 49000000.0l);
    TEST_VSPRINTF( "%.7LG",       "4.9E+07", (long double) 49000000.0l);

    TEST_VSPRINTF( "%.5A",        "-0X1.A36E3P-10",    (double) -1.6e-3  );
    TEST_VSPRINTF( "%#.5r1LE",    "-0X6.8DB8C~-03",    (long double) -1.6e-3  );
    TEST_VSPRINTF( "%.8r1LF",     "-0.0068DB8C",       (long double) -1.6e-3  );
    TEST_VSPRINTF( "%#.5r1E",     "-0X6.8DB8C~-03",    (double) -1.6e-3  );
    TEST_VSPRINTF( "%.8r1F",      "-0.0068DB8C",       (double) -1.6e-3  );
    TEST_VSPRINTF( "%#017.8r1F",  "-0X00000.0068DB8C", (double) -1.6e-3  );
    TEST_VSPRINTF( "%#017.5r1E",  "-0X0006.8DB8C~-03", (double) -1.6e-3  );
    TEST_VSPRINTF( "%#017.8r1LF", "-0X00000.0068DB8C", (long double) -1.6e-3  );
    TEST_VSPRINTF( "%#017.5r1LE", "-0X0006.8DB8C~-03", (long double) -1.6e-3  );
    TEST_VSPRINTF( "%#017.8r1f",  "-0x00000.0068db8c", (double) -1.6e-3  );
    TEST_VSPRINTF( "%#017.5r1e",  "-0x0006.8db8c~-03", (double) -1.6e-3  );
    TEST_VSPRINTF( "%#017.8r1Lf", "-0x00000.0068db8c", (long double) -1.6e-3  );
    TEST_VSPRINTF( "%#017.5r1Le", "-0x0006.8db8c~-03", (long double) -1.6e-3  );
    TEST_VSPRINTF( "%017.8r1f",   "-0000000.0068db8c", (double) -1.6e-3  );
    TEST_VSPRINTF( "%017.5r1e",   "-000006.8db8c~-03", (double) -1.6e-3  );
    TEST_VSPRINTF( "%017.8r1Lf",  "-0000000.0068db8c", (long double) -1.6e-3  );
    TEST_VSPRINTF( "%017.5r1Le",  "-000006.8db8c~-03", (long double) -1.6e-3  );
    TEST_VSPRINTF( "%017.8r1Lf",  "00000000.00000000", (long double)  0.0  );
    TEST_VSPRINTF( "%017.8r1Lf",  "00000001.00000000", (long double)  1.0  );
    TEST_VSPRINTF( "%017.8r1Lf",  "-0000001.00000000", (long double)  -1.0  );
    TEST_VSPRINTF( "%17.8r1Lf",   "      -1.00000000", (long double)  -1.0  );
    TEST_VSPRINTF( "%17.8r1Le",   "  -1.40000000~+00", (long double)  -1.25  );

    TEST_VSPRINTF( "%#-17.8r1G",  "-0X0.0068DB8BAC  ", (double) -1.6e-3  );
    TEST_VSPRINTF( "%-17.8r1G",   "-0.0068DB8BAC    ", (double) -1.6e-3  );
    TEST_VSPRINTF( "%#17.8r1G",   "  -0X0.0068DB8BAC", (double) -1.6e-3  );
    TEST_VSPRINTF( "%17.8r1G",    "    -0.0068DB8BAC", (double) -1.6e-3  );
    TEST_VSPRINTF( "%#017.8r1G",  "-0X000.0068DB8BAC", (double) -1.6e-3  );
    TEST_VSPRINTF( "%017.8r1G",   "-00000.0068DB8BAC", (double) -1.6e-3  );
    TEST_VSPRINTF( "%#017.8r1Lg", "-0x000.0068db8bac", (long double) -1.6e-3  );
    TEST_VSPRINTF( "%017.8r1Lg",  "-00000.0068db8bac", (long double) -1.6e-3  );

    TEST_VSPRINTF( "%#.5r2LE",    "-0B1.00101E-100",   (long double) -7.2e-2  );
    TEST_VSPRINTF( "%.8r2LF",     "-0.00010010",       (long double) -7.2e-2  );
    TEST_VSPRINTF( "%#.5r2E",     "-0B1.00101E-100",   (double) -7.2e-2  );
    TEST_VSPRINTF( "%.8r2F",      "-0.00010010",       (double) -7.2e-2  );
    TEST_VSPRINTF( "%#-17.8r2G",  "-0B0.00010010011 ", (double) -7.2e-2  );
    TEST_VSPRINTF( "%-17.8r2G",   "-0.00010010011   ", (double) -7.2e-2 );
    TEST_VSPRINTF( "%#17.8r2G",   " -0B0.00010010011", (double) -7.2e-2  );
    TEST_VSPRINTF( "%#17.8r2G",   "  0B0.00010010011", (double)  7.2e-2  );
    TEST_VSPRINTF( "%17.8r2G",    "   -0.00010010011", (double) -7.2e-2  );
    TEST_VSPRINTF( "%#017.8r2G",  "-0B00.00010010011", (double) -7.2e-2  );
    TEST_VSPRINTF( "%017.8r2G",   "-0000.00010010011", (double) -7.2e-2  );
    TEST_VSPRINTF( "%#017.8r2Lg", "-0b00.00010010011", (long double) -7.2e-2  );
    TEST_VSPRINTF( "%017.8r2Lg",  "-0000.00010010011", (long double) -7.2e-2  );
    TEST_VSPRINTF( "%017.8r2Lg",  "00000.00010010011", (long double)  7.2e-2  );
    TEST_VSPRINTF( "%017.8r2Lf",  "00000000.00000000", (long double)  0.0  );
    TEST_VSPRINTF( "%017.8r2Lf",  "00000001.00000000", (long double)  1.0  );
    TEST_VSPRINTF( "%017.8r2Lf",  "-0000001.00000000", (long double)  -1.0  );
    TEST_VSPRINTF( "%17.8r2Lf",   "      -1.00000000", (long double)  -1.0  );
    TEST_VSPRINTF( "%17.8r2Le",   "  -1.01000000e+00", (long double)  -1.25  );

    TEST_VSPRINTF( "%5.2r1LE%.0r3Lf", " -NAN-10", (long double) nnan ARG((long double) -3.0));
    TEST_VSPRINTF( "%5.2r2LE%.0r2Lf", " -NAN-11", (long double) nnan ARG((long double) -3.0));

    TEST_VSPRINTF("%+5.10r2E %+10.10r2e %+10.10r7e %+10.10r4e %+10.0e %+10.0e ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
                   (double) 3.9 ARG(7.89456123e-307) ARG(7.89456123e+307) ARG(789456123.0) ARG(0.789456123) ARG(DBL_MIN));

    TEST_VSPRINTF("%+5.10r2LE %+10.10r2Le %+10.10r7Le %+10.10r4Le %+10.0Le %+10.0Le ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
                   (long double) 3.9 ARG((long double) 7.89456123e-307) ARG((long double) 7.89456123e+307) ARG((long double) 789456123.0) ARG((long double) 0.789456123) ARG((long double) DBL_MIN));

    TEST_VSPRINTF("%+5.10r*E %+10.10r*e %+10.10r*e %+10.10r*e %+10.0e %+10.0e ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
                   (int) 2 ARG(3.9) ARG((int) 2) ARG(7.89456123e-307) ARG((int) 7) ARG(7.89456123e+307) ARG((int) 4) ARG(789456123.0) ARG(0.789456123) ARG(DBL_MIN));

    TEST_VSPRINTF("%+5.10r*LE %+10.10r*Le %+10.10r*Le %+10.10r*Le %+10.0Le %+10.0Le ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
                   (int) 2 ARG((long double) 3.9) ARG((int) 2) ARG((long double) 7.89456123e-307) ARG((int) 7) ARG((long double) 7.89456123e+307) ARG((int) 4) ARG((long double) 789456123.0) ARG((long double) 0.789456123) ARG((long double) DBL_MIN));

    TEST_VSPRINTF( "%r0I16d",     "100",                       (int16_t) 100 );
    TEST_VSPRINTF( "%r1I16d",     "64",                        (int16_t) 100 );
    TEST_VSPRINTF( "%r*I16d",     "-1000",                     (int) 5 ARG((int16_t) -125));

    TEST_VSPRINTF( "%r0I32d",     "100",                       (int32_t) 100 );
    TEST_VSPRINTF( "%r1I32d",     "64",                        (int32_t) 100 );
    TEST_VSPRINTF( "%r*I32d",     "-1000",                     (int) 5 ARG((int32_t) -125));

    TEST_VSPRINTF( "%r0I64d",     "100",                       (int64_t) 100 );
    TEST_VSPRINTF( "%r1I64d",     "64",                        (int64_t) 100 );
    TEST_VSPRINTF( "%r*I64d",     "-1000",                     (int) 5 ARG((int64_t) -125));

    TEST_VSPRINTF( "%r0I16u",     "100",                       (uint16_t) 100 );
    TEST_VSPRINTF( "%r1I16u",     "64",                        (uint16_t) 100 );
    TEST_VSPRINTF( "%r*I16u",     "1000",                      (int) 5 ARG((uint16_t) 125));

    TEST_VSPRINTF( "%r0I32u",     "100",                       (uint32_t) 100 );
    TEST_VSPRINTF( "%r1I32u",     "64",                        (uint32_t) 100 );
    TEST_VSPRINTF( "%r*I32u",     "1000",                      (int) 5 ARG((uint32_t) 125));

    TEST_VSPRINTF( "%r0I64u",     "100",                       (int64_t) 100 );
    TEST_VSPRINTF( "%r1I64u",     "64",                        (int64_t) 100 );
    TEST_VSPRINTF( "%r*I64u",     "1000",                      (int) 5 ARG((uint64_t) 125));

    TEST_VSPRINTF( "%r0d",        "100",                       (int) 100 );
    TEST_VSPRINTF( "%r1d",        "64",                        (int) 100 );
    TEST_VSPRINTF( "%r*d",        "-1000",                     (int) 5 ARG((int) -125));

    TEST_VSPRINTF( "%r0u",        "100",                       (unsigned int) 100 );
    TEST_VSPRINTF( "%r1u",        "64",                        (unsigned int) 100 );
    TEST_VSPRINTF( "%r*u"   ,     "1000",                      (int) 5 ARG((unsigned int) 125));

    TEST_VSPRINTF( "%r0ld",       "100",                       (long) 100 );
    TEST_VSPRINTF( "%r1ld",       "64",                        (long) 100 );
    TEST_VSPRINTF( "%r*ld",       "-1000",                     (int) 5 ARG((long) -125));

    TEST_VSPRINTF( "%r0lu",       "100",                       (unsigned long) 100 );
    TEST_VSPRINTF( "%r1lu",       "64",                        (unsigned long) 100 );
    TEST_VSPRINTF( "%r*lu",       "1000",                      (int) 5 ARG((unsigned long) 125));

    TEST_VSPRINTF( "%r0lld",      "100",                       (long long) 100 );
    TEST_VSPRINTF( "%r1lld",      "64",                        (long long) 100 );
    TEST_VSPRINTF( "%r*lld",      "-1000",                     (int) 5 ARG((long long) -125));

    TEST_VSPRINTF( "%r0llu",      "100",                       (unsigned long long) 100 );
    TEST_VSPRINTF( "%r1llu",      "64",                        (unsigned long long) 100 );
    TEST_VSPRINTF( "%r*llu",      "1000",                      (int) 5 ARG((unsigned long long) 125));

    TEST_VSPRINTF( "%r0l1u",      "100",                       (uint8_t) 100 );
    TEST_VSPRINTF( "%r1l1u",      "64",                        (uint8_t) 100 );
    TEST_VSPRINTF( "%r*l1u",      "1000",                      (int) 5 ARG((uint8_t) 125));

    TEST_VSPRINTF( "%r0l1d",      "100",                       (int8_t) 100 );
    TEST_VSPRINTF( "%r1l1d",      "64",                        (int8_t) 100 );
    TEST_VSPRINTF( "%r*l1d",      "-1000",                     (int) 5 ARG((int8_t) -125));

    TEST_VSPRINTF( "%r0l2u",      "100",                       (uint16_t) 100 );
    TEST_VSPRINTF( "%r1l2u",      "64",                        (uint16_t) 100 );
    TEST_VSPRINTF( "%r*l2u",      "1000",                      (int) 5 ARG((uint16_t) 125));

    TEST_VSPRINTF( "%r0l2d",      "100",                       (int16_t) 100 );
    TEST_VSPRINTF( "%r1l2d",      "64",                        (int16_t) 100 );
    TEST_VSPRINTF( "%r*l2d",      "-1000",                     (int) 5 ARG((int16_t) -125));

    TEST_VSPRINTF( "%r0l4u",      "100",                       (uint32_t) 100 );
    TEST_VSPRINTF( "%r1l4u",      "64",                        (uint32_t) 100 );
    TEST_VSPRINTF( "%r*l4u",      "1000",                      (int) 5 ARG((uint32_t) 125));

    TEST_VSPRINTF( "%r0l4d",      "100",                       (int32_t) 100 );
    TEST_VSPRINTF( "%r1l4d",      "64",                        (int32_t) 100 );
    TEST_VSPRINTF( "%r*l4d",      "-1000",                     (int) 5 ARG((int32_t) -125));

    TEST_VSPRINTF( "%r0l8u",      "100",                       (uint64_t) 100 );
    TEST_VSPRINTF( "%r1l8u",      "64",                        (uint64_t) 100 );
    TEST_VSPRINTF( "%r*l8u",      "1000",                      (int) 5 ARG((uint64_t) 125));

    TEST_VSPRINTF( "%r0l8d",      "100",                       (int64_t) 100 );
    TEST_VSPRINTF( "%r1l8d",      "64",                        (int64_t) 100 );
    TEST_VSPRINTF( "%r*l8d",      "-1000",                     (int) 5 ARG((int64_t) -125));

    TEST_VSPRINTF( "%r1hhd",      "-64",                        (int) -100 );
    TEST_VSPRINTF( "%r1hd",       "-64",                        (int) -100 );
    TEST_VSPRINTF( "%r1ld",       "-64",                        (long) -100 );
    TEST_VSPRINTF( "%r1lld",      "-64",                        (long long) -100 );

    TEST_VSPRINTF( "%r0hhd",      "-100",                        (int) -100 );
    TEST_VSPRINTF( "%r0hd",       "-100",                        (int) -100 );
    TEST_VSPRINTF( "%r0ld",       "-100",                        (long) -100 );
    TEST_VSPRINTF( "%r0lld",      "-100",                        (long long) -100 );

    TEST_VSPRINTF( "%r*hhd",      "-1000",                      (int) 5 ARG((int) -125));
    TEST_VSPRINTF( "%r*hd",       "-1000",                      (int) 5 ARG((int) -125));
    TEST_VSPRINTF( "%r*ld",       "-1000",                      (int) 5 ARG((long) -125));
    TEST_VSPRINTF( "%r*lld",      "-1000",                      (int) 5 ARG((long long) -125));

    if(sizeof(void *) == 4)
    {
        TEST_VSPRINTF( "%p",   "1234abcd",    (void*) 0x1234abcd);
        TEST_VSPRINTF( "%P",   "1234ABCD",    (void*) 0x1234abcd);
        TEST_VSPRINTF( "%#p",  "0x1234abcd",  (void*) 0x1234abcd);
        TEST_VSPRINTF( "%#P",  "0X1234ABCD",  (void*) 0x1234abcd);
        TEST_VSPRINTF( "%10p", "  1234abcd",  (void*) 0x1234abcd);
        TEST_VSPRINTF( "%10P", "  1234ABCD",  (void*) 0x1234abcd);
    }
    else if(sizeof(void *) == 8)
    {
        TEST_VSPRINTF( "%p",   "1234567890abcdef",   (void*) 0x1234567890abcdef);
        TEST_VSPRINTF( "%P",   "1234567890ABCDEF",   (void*) 0x1234567890abcdef);
        TEST_VSPRINTF( "%#p",  "0x1234567890abcdef", (void*) 0x1234567890abcdef);
        TEST_VSPRINTF( "%#P",  "0X1234567890ABCDEF", (void*) 0x1234567890abcdef);
        TEST_VSPRINTF( "%18p", "  1234567890abcdef", (void*) 0x1234567890abcdef);
        TEST_VSPRINTF( "%18P", "  1234567890ABCDEF", (void*) 0x1234567890abcdef);
    }

    TEST_VSPRINTF( "%%v: %v", "%v: Hello world!", &cbfunc ARG(&cbdata));
    TEST_VSPRINTF( "#%%v: %v%s#", "#%v: Hello world!?#", &cbfunc ARG(&cbdata) ARG("?"));
    TEST_VSPRINTF( "%%#-12.5v: %#-12.5v", "%#-12.5v: Hello       ", &cbfunc ARG(&cbdata));
    TEST_VSPRINTF( "%%#12.5v: %#12.5v", "%#12.5v:        Hello", &cbfunc ARG(&cbdata));
    TEST_VSPRINTF( "%%*.*v: %*.*v", "%*.*v:        Hello", (int)12 ARG((int)5) ARG(&cbfunc) ARG(&cbdata));
    TEST_VSPRINTF( "%%*.*v: %*.*v", "%*.*v: Hello       ", (int)-12 ARG((int)5) ARG(&cbfunc) ARG(&cbdata));

    TEST_VSPRINTF( "%%V: %V", "%V: Hello world!", &cbdata);
    TEST_VSPRINTF( "#%%V: %V%s#", "#%V: Hello world!?#", &cbdata ARG("?"));
    TEST_VSPRINTF( "%%#-12.5V: %#-12.5V", "%#-12.5V: Hello       ", &cbdata);
    TEST_VSPRINTF( "%%#12.5V: %#12.5V", "%#12.5V:        Hello", &cbdata);
    TEST_VSPRINTF( "%%*.*V: %*.*V", "%*.*V:        Hello", (int)12 ARG((int)5) ARG(&cbdata));
    TEST_VSPRINTF( "%%*.*V: %*.*V", "%*.*V: Hello       ", (int)-12 ARG((int)5) ARG(&cbdata));
 
    return (bRet);
} /* test_ssprintf() */


/* ------------------------------------------------------------------------- *\
   main function
\* ------------------------------------------------------------------------- */
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


/* ========================================================================= *\
   END OF FILE
\* ========================================================================= */
