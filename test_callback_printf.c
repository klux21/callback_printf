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


int test_ssprintf()
{
   int bRet = 1;

   static uint32_t inf  = 0x7f800000;
   static uint32_t ninf = 0xff800000;
   static uint32_t nan  = 0x7fc00000;
   static uint32_t nnan = 0xffc00000;

#define ARG(x)          ,x

#define TEST_SVSPRINTF(pfmt, pout, arg) bRet &= test_svsprintf(pout, #pfmt "," #arg, pfmt, arg);

    TEST_SVSPRINTF( "Hallo Welt!", "Hallo Welt!",  0);
    TEST_SVSPRINTF( "%.10s!",      "Hallo Welt!",  "Hallo Welt");
    TEST_SVSPRINTF( "%.*s!",       "Hallo Welt!", (int) 10 ARG("Hallo Welt"));
    TEST_SVSPRINTF( "%.5s %.5s",   "Hallo Welt!",  "Hallo" ARG("Welt!"));
    TEST_SVSPRINTF( "%-6.5s%4.5s!","Hallo Welt!",  "Hallo" ARG("Welt"));
    TEST_SVSPRINTF( "%c%c%c%c%c %c%c%c%c!", "Hallo Welt!", 'H' ARG('a') ARG('l') ARG('l') ARG('o') ARG('W') ARG('e') ARG('l') ARG('t'));
    TEST_SVSPRINTF( "%02d/%02d/%04d %02d:%02d:%02d", "01/01/1970 23:59:59", 1 ARG(1) ARG(1970) ARG(23) ARG(59) ARG(59));
    TEST_SVSPRINTF( "%.2d/%.2d/%.4d %.2d:%.2d:%.2d", "01/01/1970 23:59:59", 1 ARG(1) ARG(1970) ARG(23) ARG(59) ARG(59));
    TEST_SVSPRINTF( "%+#23.15e",   " +1.000000000000000e-01", (double) 1.0e-1 );
    TEST_SVSPRINTF( "%+#23.15e",   " +3.900000000000000e+00", (double) 3.9 );
    TEST_SVSPRINTF( "%+#23.14e",   " +7.89456123000000e-307", (double) 7.89456123e-307 );
    TEST_SVSPRINTF( "%+#23.14e",   " +7.89456123000000e+307", (double) 7.89456123e+307 );
    TEST_SVSPRINTF( "%+#23.15e",   " +7.894561230000000e+08", (double) 789456123.0 );
    TEST_SVSPRINTF( "%-#23.15e",   "7.894561230000000e+08  ", (double) 789456123.0 );
    TEST_SVSPRINTF( "%#23.15e",    "  7.894561230000000e+08", (double)789456123.0 );
    TEST_SVSPRINTF( "%#1.1g",      "8.e+08", (double) 789456123.0 );
    TEST_SVSPRINTF( "%+#23.15Le",  " +1.000000000000000e-01", (long double) 1.0e-1l );
    TEST_SVSPRINTF( "%+#23.15Le",  " +3.900000000000000e+00", (long double) 3.9l );
    TEST_SVSPRINTF( "%+#27.6e",    "             +1.594561e-317", (double) 1.59456123e-317 );
    TEST_SVSPRINTF( "%+#27.16e",    "   +1.5945612300000000e+308", (double) 1.59456123e+308 );
#ifdef _WIN32
    TEST_SVSPRINTF( "%+#27.15Le",    "   +9.89456123000000000e-307",(long double) 9.89456123e-307l );
    TEST_SVSPRINTF( "%+#27.15Le",    "   +9.89456123000000000e+307",(long double) 9.89456123e+307l );
#else
    TEST_SVSPRINTF( "%+#24.14Le",  " +7.89456123000000e-4307", (long double) 7.89456123e-4307l );
    TEST_SVSPRINTF( "%+#24.14Le",  " +7.89456123000000e+4307", (long double) 7.89456123e+4307l );
#endif
    TEST_SVSPRINTF( "%+#23.15Le",  " +7.894561230000000e+08", (long double) 789456123.0l );
    TEST_SVSPRINTF( "%-#23.15Le",  "7.894561230000000e+08  ", (long double) 789456123.0l );
    TEST_SVSPRINTF( "%#23.15Le",   "  7.894561230000000e+08", (long double) 789456123.0l );
    TEST_SVSPRINTF( "%#1.1Lg",     "8.e+08",                  (long double) 789456123.0l );
    TEST_SVSPRINTF( "%I64d",       "-8589934591",             (unsigned long long) ((unsigned long long)0xffffffff)*0xffffffff );
    TEST_SVSPRINTF( "%+8I64d",     "    +100",                (unsigned long long) 100 );
    TEST_SVSPRINTF( "%+.8I64d",    "+00000100",               (unsigned long long) 100 );
    TEST_SVSPRINTF( "%+10.8I64d",  " +00000100",              (unsigned long long) 100 );
    TEST_SVSPRINTF( "%_1I64d",     "",                        (unsigned long long) 100 );
    TEST_SVSPRINTF( "%_1lld",     "",                         (unsigned long long) 100 );
    TEST_SVSPRINTF( "%-1.5I64d",   "-00100",                  (unsigned long long) -100 );
    TEST_SVSPRINTF( "%5I64d",      "  100",                   (unsigned long long) 100 );
    TEST_SVSPRINTF( "%5I64d",      " -100",                   (unsigned long long) -100 );
    TEST_SVSPRINTF( "%-5I64d",     "100  ",                   (unsigned long long) 100 );
    TEST_SVSPRINTF( "%-5I64d",     "-100 ",                   (unsigned long long) -100 );
    TEST_SVSPRINTF( "%-.5I64d",    "00100",                   (unsigned long long) 100 );
    TEST_SVSPRINTF( "%-.5I64d",    "-00100",                  (unsigned long long) -100 );
    TEST_SVSPRINTF( "%-8.5I64d",   "00100   ",                (unsigned long long) 100 );
    TEST_SVSPRINTF( "%-8.5I64d",   "-00100  ",                (unsigned long long) -100 );
    TEST_SVSPRINTF( "%05I64d",     "00100",                   (unsigned long long) 100 );
    TEST_SVSPRINTF( "%05I64d",     "-0100",                   (unsigned long long) -100 );
    TEST_SVSPRINTF( "% I64d",      " 100",                    (unsigned long long) 100 );
    TEST_SVSPRINTF( "% I64d",      "-100",                    (unsigned long long) -100 );
    TEST_SVSPRINTF( "% 5I64d",     "  100",                   (unsigned long long)  100 );
    TEST_SVSPRINTF( "% 5I64d",     " -100",                   (unsigned long long)  -100 );
    TEST_SVSPRINTF( "% .5I64d",    " 00100",                  (unsigned long long) 100 );
    TEST_SVSPRINTF( "% .5I64d",    "-00100",                  (unsigned long long) -100 );
    TEST_SVSPRINTF( "% 8.5I64d",   "   00100",                (unsigned long long) 100 );
    TEST_SVSPRINTF( "% 8.5I64d",   "  -00100",                (unsigned long long) -100 );
    TEST_SVSPRINTF( "%.0I64d",     "",                        (unsigned long long) 0 );
    TEST_SVSPRINTF( "%.0lld",        "",                          (unsigned long long) 0 );
    TEST_SVSPRINTF( "%8.0lld",       "        ",                  (unsigned long long) 0 );
    TEST_SVSPRINTF( "%08.0lld",      "        ",                  (unsigned long long) 0 );
    TEST_SVSPRINTF( "%#+21.18I64x",  " 0x00ffffffffffffff9c",     (unsigned long long) -100 );
    TEST_SVSPRINTF( "%#.25I64o",     "0001777777777777777777634", (unsigned long long) -100 );
    TEST_SVSPRINTF( "%#+24.20I64o",  " 01777777777777777777634",  (unsigned long long) -100 );
    TEST_SVSPRINTF( "%#+18.21I64X",  "0X00000FFFFFFFFFFFFFF9C",   (unsigned long long) -100 );
    TEST_SVSPRINTF( "%#+20.24I64o",  "001777777777777777777634",  (unsigned long long) -100 );
    TEST_SVSPRINTF( "%#+25.22I64u",  "   0018446744073709551615", (unsigned long long) -1 );
    TEST_SVSPRINTF( "%#+25.22I64u",  "   0018446744073709551615", (unsigned long long) -1 );
    TEST_SVSPRINTF( "%#+30.25I64u",  "     0000018446744073709551615", (unsigned long long) -1 );
    TEST_SVSPRINTF( "%+#25.22I64d",  "  -0000000000000000000001", (unsigned long long) -1 );
    TEST_SVSPRINTF( "%#-8.5I64o",    "00144   ",                  (unsigned long long) 100 );
    TEST_SVSPRINTF( "%#-+ 08.5I64d", "+00100  ",                  (unsigned long long) 100 );
    TEST_SVSPRINTF( "%.80I64d",      "00000000000000000000000000000000000000000000000000000000000000000000000000000001",  (unsigned long long) 1 );
    TEST_SVSPRINTF( "% .80I64d",     " 00000000000000000000000000000000000000000000000000000000000000000000000000000001", (unsigned long long) 1 );
    TEST_SVSPRINTF( "% .80d",        " 00000000000000000000000000000000000000000000000000000000000000000000000000000001", (int) 1 );
    TEST_SVSPRINTF( "%I",            "" , (int) 1 );
    TEST_SVSPRINTF( "%Iq",           "" , (int) 1 );
    TEST_SVSPRINTF( "%Ihd",          "" , (int) 1 );
    TEST_SVSPRINTF( "%I0d",          "" , (int) 1 );
    TEST_SVSPRINTF( "%I64D",         "" , (unsigned long long) -1 );
    TEST_SVSPRINTF( "%zx",           "1",   (size_t) 1 );
    TEST_SVSPRINTF( "%zx",           sizeof(size_t) > 4 ? "ffffffffffffffff" : "ffffffff", (size_t) -1 );
    TEST_SVSPRINTF( "%z",            "" ,   (size_t) 1 );
    TEST_SVSPRINTF( "%tx",           "1",   (ptrdiff_t) 1 );
    TEST_SVSPRINTF( "%tx",           sizeof(size_t) > 4 ? "8000000000000000" : "80000000", (ptrdiff_t) 1 << (sizeof(ptrdiff_t) * 8 - 1));
    TEST_SVSPRINTF( "%t",            "" ,   (ptrdiff_t) 1 );
    TEST_SVSPRINTF( "% d",           " 1",  (int) 1 );
    TEST_SVSPRINTF( "%+ d",          "+1",  (int) 1 );
    TEST_SVSPRINTF( "%S",            "wide", (wchar_t *) L"wide" );
    TEST_SVSPRINTF( "%04c",          "   1", (int) '1' );
    TEST_SVSPRINTF( "%-04c",         "1   ", (int) '1' );
    TEST_SVSPRINTF( "%#012x",        "0x0000000001", (int) 1 );
    TEST_SVSPRINTF( "%#012x",        "000000000000", (int) 0 );
    TEST_SVSPRINTF( "%#04.8x",       "0x00000001",   (int) 1 );
    TEST_SVSPRINTF( "%#04.8x",       "00000000",     (int) 0 );
    TEST_SVSPRINTF( "%#-08.2x",      "0x01    ",     (int) 1 );
    TEST_SVSPRINTF( "%#-08.2x",      "00      ",     (int) 0 );
    TEST_SVSPRINTF( "%#.0x",         "0x1",          (int) 1 );
    TEST_SVSPRINTF( "%#.0x",         "",            (int) 0 );
    TEST_SVSPRINTF( "%#08o",         "00000001",    (int) 1 );
    TEST_SVSPRINTF( "%#o",           "01",          (int) 1 );
    TEST_SVSPRINTF( "%#o",           "0",           (int) 0 );
    TEST_SVSPRINTF( "%04s",          " foo",        (char *) "foo" );
    TEST_SVSPRINTF( "%.1s",          "f",           (char *) "foo" );
    TEST_SVSPRINTF( "%.0s",          "",            (char *) "foo" );
    TEST_SVSPRINTF( "hello",         "hello",        0 );
    TEST_SVSPRINTF( "%ws",           "",            (wchar_t *) L"wide" );
    TEST_SVSPRINTF( "%-10ws",        "",            (wchar_t *) L"wide" );
    TEST_SVSPRINTF( "%10ws",         "",            (wchar_t *) L"wide" );
    TEST_SVSPRINTF( "%#+ -03whlls",  "",            (wchar_t *) L"wide" );
    TEST_SVSPRINTF( "%w0s",          "",            (wchar_t *) L"wide" );
    TEST_SVSPRINTF( "%w-s",          "",            (wchar_t *) L"wide" );
    TEST_SVSPRINTF( "%ls",           "wide",        (wchar_t *) L"wide" );
    TEST_SVSPRINTF( "%Ls",           "",            (wchar_t *) "not wide" );
    TEST_SVSPRINTF( "%6.4c",         "  ____",      (int) '_' );
    TEST_SVSPRINTF( "%-6.4c",         "____  ",     (int) '_' );
    TEST_SVSPRINTF( "%b",            "101010",      (int) 42 );
    TEST_SVSPRINTF( "%#b",           "0b101010",    (int) 42 );
    TEST_SVSPRINTF( "%#B",           "0B101010",    (int) 42 );
    TEST_SVSPRINTF( "%3c",           "  a",         (int) 'a' );
    TEST_SVSPRINTF( "%-3c",          "a  ",         (int) 'a' );
    TEST_SVSPRINTF( "%3d",           "1234",        (int) 1234 );
    TEST_SVSPRINTF( "%3h",           "",             0 );
    TEST_SVSPRINTF( "%k%m%q%r%t%v%y%z", "",          0 );
    TEST_SVSPRINTF( "%-1d",          "2",              (int) 2 );
    TEST_SVSPRINTF( "%2.4f",         "8.6000",         (double) 8.6 );
    TEST_SVSPRINTF( "%0f",           "0.600000",       (double) 0.6 );
    TEST_SVSPRINTF( "%.0f",          "1",              (double) 0.6 );
    TEST_SVSPRINTF( "%2.4e",         "8.6000e+00",     (double) 8.6 );
    TEST_SVSPRINTF( "% 2.4e",        " 8.6000e+00",    (double) 8.6 );
    TEST_SVSPRINTF( "% 014.4e",      " 0008.6000e+00", (double) 8.6 );
    TEST_SVSPRINTF( "% 2.4e",        "-8.6000e+00",    (double)  -8.6 );
    TEST_SVSPRINTF( "%+2.4e",        "+8.6000e+00",    (double) 8.6 );
    TEST_SVSPRINTF( "%2.4g",         "8.6",            (double) 8.6 );
    TEST_SVSPRINTF( "%2.4Lf",        "8.6000",         (long double) 8.6 );
    TEST_SVSPRINTF( "%0Lf",          "0.600000",       (long double) 0.6 );
    TEST_SVSPRINTF( "%.0Lf",         "1",              (long double) 0.6 );
    TEST_SVSPRINTF( "%2.4Le",        "8.6000e+00",     (long double) 8.6 );
    TEST_SVSPRINTF( "% 2.4Le",       " 8.6000e+00",    (long double) 8.6 );
    TEST_SVSPRINTF( "% 014.4Le",     " 0008.6000e+00", (long double) 8.6 );
    TEST_SVSPRINTF( "% 2.4Le",       "-8.6000e+00",    (long double)  -8.6 );
    TEST_SVSPRINTF( "%+2.4Le",       "+8.6000e+00",    (long double) 8.6 );
    TEST_SVSPRINTF( "%2.4Lg",        "8.6",            (long double) 8.6 );
    TEST_SVSPRINTF( "%-i",           "-1",             (int) -1 );
    TEST_SVSPRINTF( "%-i",           "1",              (int) 1 );
    TEST_SVSPRINTF( "%+i",           "+1",             (int) 1 );
    TEST_SVSPRINTF( "%o",            "12",             (int) 10 );
    TEST_SVSPRINTF( "%s",            "<NULL>",         (char *) NULL );
    TEST_SVSPRINTF( "%s",            "%%%%",           (char *) "%%%%" );
    TEST_SVSPRINTF( "%u",            "4294967295",     (int) -1 );
    TEST_SVSPRINTF( "%u",            "2147483648",     (unsigned int) 0x80000000 );
    TEST_SVSPRINTF( "%d",            "-2147483648",    (int) 0x80000000 );
    TEST_SVSPRINTF( "%w",            "",               (int) -1 );
    TEST_SVSPRINTF( "%h",            "",               (int) -1 );
    TEST_SVSPRINTF( "%j",            "",               (intmax_t) -1 );
    TEST_SVSPRINTF( "%jd",           "-1",             (intmax_t) -1 );
    TEST_SVSPRINTF( "%F",            "0.000000",       (double) 0 );
    TEST_SVSPRINTF( "%N",            "",               (int) -1 );
    TEST_SVSPRINTF( "%H",            "",               (int) -1 );
    TEST_SVSPRINTF( "x%cx",          "xXx",            (int) 0x100+'X' );
    TEST_SVSPRINTF( "%%0",           "%0",              0 );
    TEST_SVSPRINTF( "%%%%%%%%",      "%%%%",            0 );
    TEST_SVSPRINTF( "%%%%%d%%%%",    "%%42%%",         (int) 42 );
    TEST_SVSPRINTF( "%hx",           "2345",           (int) 0x12345 );
    TEST_SVSPRINTF( "%hhx",          "23",             (int) 0x123 );
    TEST_SVSPRINTF( "%hhx",          "45",             (int) 0x12345 );
    TEST_SVSPRINTF( "%c",            "a",              (int) 'a' );
    TEST_SVSPRINTF( "%c",            "\x82",           (int) 0xa082 );
    TEST_SVSPRINTF( "%C",            "a",              (int) 'a' );
    TEST_SVSPRINTF( "%C",            "\xe3\x81\x82" /*""*/, (int) 0x3042 );
    TEST_SVSPRINTF( "%l4c",          "\xe3\x81\x82" /*""*/, (int) 0x3042 );
    TEST_SVSPRINTF( "%l2c",          "\xe3\x81\x82" /*""*/, (int) 0x3042 );
    TEST_SVSPRINTF( "a%Cb",          "a\xe3\x81\x82" "b",   (int) 0x3042 );
    TEST_SVSPRINTF( "%lld",          "-8589934591",   (unsigned long long) ((unsigned long long)0xffffffff)*0xffffffff );
    TEST_SVSPRINTF( "%llu",          "18446744065119617025", (unsigned long long) ((unsigned long long)0xffffffff)*0xffffffff );
    TEST_SVSPRINTF( "%I32d",         "1",             (int) 1 );
    TEST_SVSPRINTF( "%.0f",          "-2",            (double) -1.5 );
    TEST_SVSPRINTF( "%.0f",          "-1",            (double) -0.5 );
    TEST_SVSPRINTF( "%.0f",          "1",             (double) 0.5 );
    TEST_SVSPRINTF( "%.0f",          "2",             (double) 1.5 );

    TEST_SVSPRINTF( "%.3e", "0.000e+00",    (double) 0.0 );
    TEST_SVSPRINTF( "%.3e", "5.000e+00",    (double) 5e-0 );
    TEST_SVSPRINTF( "%.3e", "5.000e+00",    (double) 5e+0 );
    TEST_SVSPRINTF( "%.3e", "5.000e-01",    (double) 5e-1 );
    TEST_SVSPRINTF( "%.3e", "5.000e+01",    (double) 5e+1 );
    TEST_SVSPRINTF( "%.3e", "5.000e-02",    (double) 5e-2 );
    TEST_SVSPRINTF( "%.3e", "5.000e+02",    (double) 5e+2 );
    TEST_SVSPRINTF( "%.3e", "5.000e-03",    (double) 5e-3 );
    TEST_SVSPRINTF( "%.3e", "5.000e+03",    (double) 5e+3 );
    TEST_SVSPRINTF( "%.3e", "9.995e+00",    (double) 9.995e-0 );
    TEST_SVSPRINTF( "%.3e", "9.995e+00",    (double) 9.995e+0 );
    TEST_SVSPRINTF( "%.3e", "9.995e-01",    (double) 9.995e-1 );
    TEST_SVSPRINTF( "%.3e", "9.995e+01",    (double) 9.995e+1 );
    TEST_SVSPRINTF( "%.3e", "9.995e-02",    (double) 9.995e-2 );
    TEST_SVSPRINTF( "%.3e", "9.995e+02",    (double) 9.995e+2 );
    TEST_SVSPRINTF( "%.2e", "1.00e+03",     (double) 9.9951e+2 );
    TEST_SVSPRINTF( "%.2e", "-1.00e+03",    (double) -9.9951e+2 );
    TEST_SVSPRINTF( "%.2e", "1.00e-01",     (double) 9.9951e-2 );
    TEST_SVSPRINTF( "%.2e", "-1.00e-01",    (double) -9.9951e-2 );
    TEST_SVSPRINTF( "%#.0e", "1.e+03",      (double) 9.51e+2 );
    TEST_SVSPRINTF( "%#.0e", "-1.e+03",     (double) -9.51e+2 );
    TEST_SVSPRINTF( "%#.0e", "1.e-01",      (double) 9.51e-2 );
    TEST_SVSPRINTF( "%#.0e", "-1.e-01",     (double) -9.51e-2 );
    TEST_SVSPRINTF( "%.3e", "1.000e+00",    (double)  1.0 );
    TEST_SVSPRINTF( "%.3e", "-1.000e+00",   (double) -1.0 );
    TEST_SVSPRINTF( "%.3e", "1.000e+01",    (double)  1e+1 );
    TEST_SVSPRINTF( "%.3e", "-1.000e-01",   (double) -1e-1 );
    TEST_SVSPRINTF( "%.3e", "-1.000e+01",   (double) -1e+1 );
    TEST_SVSPRINTF( "%.3e", "1.000e-01",    (double)  1e-1 );
    TEST_SVSPRINTF( "%.3e", "-1.000e-01",   (double) -1e-1 );
    TEST_SVSPRINTF( "%.3e", "1.000e+02",    (double)  1e+2 );
    TEST_SVSPRINTF( "%.3e", "1.000e-02",    (double)  1e-2 );
    TEST_SVSPRINTF( "%.3e", "1.000e+03",    (double)  1e+3 );
    TEST_SVSPRINTF( "%.3e", "1.000e-03",    (double)  1e-3 );

    TEST_SVSPRINTF( "%.3Le", "0.000e+00",   (long double) 0.0 );
    TEST_SVSPRINTF( "%.3Le", "5.000e+00",   (long double) 5e-0 );
    TEST_SVSPRINTF( "%.3Le", "5.000e+00",   (long double) 5e+0 );
    TEST_SVSPRINTF( "%.3Le", "5.000e-01",   (long double) 5e-1 );
    TEST_SVSPRINTF( "%.3Le", "5.000e+01",   (long double) 5e+1 );
    TEST_SVSPRINTF( "%.3Le", "5.000e-02",   (long double) 5e-2 );
    TEST_SVSPRINTF( "%.3Le", "5.000e+02",   (long double) 5e+2 );
    TEST_SVSPRINTF( "%.3Le", "5.000e-03",   (long double) 5e-3 );
    TEST_SVSPRINTF( "%.3Le", "5.000e+03",   (long double) 5e+3 );
    TEST_SVSPRINTF( "%.3Le", "9.995e+00",   (long double) 9.995e-0 );
    TEST_SVSPRINTF( "%.3Le", "9.995e+00",   (long double) 9.995e+0 );
    TEST_SVSPRINTF( "%.3Le", "9.995e-01",   (long double) 9.995e-1 );
    TEST_SVSPRINTF( "%.3Le", "9.995e+01",   (long double) 9.995e+1 );
    TEST_SVSPRINTF( "%.3Le", "9.995e-02",   (long double) 9.995e-2 );
    TEST_SVSPRINTF( "%.3Le", "9.995e+02",   (long double) 9.995e+2 );
    TEST_SVSPRINTF( "%#.0Le", "1.e+03",     (long double) 9.51e+2 );
    TEST_SVSPRINTF( "%#.0Le", "-1.e+03",    (long double) -9.51e+2 );
    TEST_SVSPRINTF( "%#.0Le", "1.e-01",     (long double) 9.51e-2 );
    TEST_SVSPRINTF( "%#.0Le", "-1.e-01",    (long double) -9.51e-2 );
    TEST_SVSPRINTF( "%.3Le", "1.000e+00",   (long double)  1.0 );
    TEST_SVSPRINTF( "%.3Le", "-1.000e+00",  (long double) -1.0 );
    TEST_SVSPRINTF( "%.3Le", "1.000e+01",   (long double)  1e+1 );
    TEST_SVSPRINTF( "%.3Le", "-1.000e-01",  (long double) -1e-1 );
    TEST_SVSPRINTF( "%.3Le", "-1.000e+01",  (long double) -1e+1 );
    TEST_SVSPRINTF( "%.3Le", "1.000e-01",   (long double)  1e-1 );
    TEST_SVSPRINTF( "%.3Le", "-1.000e-01",  (long double) -1e-1 );
    TEST_SVSPRINTF( "%.3Le", "1.000e+02",   (long double)  1e+2 );
    TEST_SVSPRINTF( "%.3Le", "1.000e-02",   (long double)  1e-2 );
    TEST_SVSPRINTF( "%.3Le", "1.000e+03",   (long double)  1e+3 );
    TEST_SVSPRINTF( "%.3Le", "1.000e-03",   (long double)  1e-3 );

    TEST_SVSPRINTF( "%.3a",  "-0x1.a37p-10",  (double)       -1.6e-3 );
    TEST_SVSPRINTF( "%.3A",  "-0X1.A37P-10",  (double)       -1.6e-3 );
    TEST_SVSPRINTF( "%.3La", "-0x1.a37p-10",  (long double)  -1.6e-3 );
    TEST_SVSPRINTF( "%.3LA", "-0X1.A37P-10",  (long double)  -1.6e-3 );

    TEST_SVSPRINTF( "%5.2e%.0f",  "  inf3",   (double) *(float*) &inf ARG(3.0));
    TEST_SVSPRINTF( "%5.2e%.0f",  " -inf3",   (double) *(float*) &ninf ARG(3.0));
    TEST_SVSPRINTF( "%5.2e%.0f",  "  nan3",   (double) *(float*) &nan ARG(3.0));
    TEST_SVSPRINTF( "%5.2e%.0f",  " -nan3",   (double) *(float*) &nnan ARG(3.0));
    TEST_SVSPRINTF( "%5.2E%.0f",  "  INF-3",  (double) *(float*) &inf ARG(-3.0));
    TEST_SVSPRINTF( "%5.2E%.0f",  " -INF-3",  (double) *(float*) &ninf ARG(-3.0));
    TEST_SVSPRINTF( "%5.2E%.0f",  "  NAN-3",  (double) *(float*) &nan ARG(-3.0));
    TEST_SVSPRINTF( "%5.2E%.0f",  " -NAN-3",  (double) *(float*) &nnan ARG(-3.0));

    TEST_SVSPRINTF( "%5.2Le%.0Lf", "  inf3",  (long double) *(float*) &inf ARG((long double) 3.0));
    TEST_SVSPRINTF( "%5.2Le%.0Lf", " -inf3",  (long double) *(float*) &ninf ARG((long double) 3.0));
    TEST_SVSPRINTF( "%5.2Le%.0Lf", "  nan3",  (long double) *(float*) &nan ARG((long double) 3.0));
    TEST_SVSPRINTF( "%5.2Le%.0Lf", " -nan3",  (long double) *(float*) &nnan ARG((long double) 3.0));
    TEST_SVSPRINTF( "%5.2LE%.0Lf", "  INF-3", (long double) *(float*) &inf ARG((long double) -3.0));
    TEST_SVSPRINTF( "%5.2LE%.0Lf", " -INF-3", (long double) *(float*) &ninf ARG((long double) -3.0));
    TEST_SVSPRINTF( "%5.2LE%.0Lf", "  NAN-3", (long double) *(float*) &nan ARG((long double) -3.0));
    TEST_SVSPRINTF( "%5.2LE%.0Lf", " -NAN-3", (long double) *(float*) &nnan ARG((long double) -3.0));

    TEST_SVSPRINTF( "%# 01.1g",  " 1.e+01",   (double) 9.8);
    TEST_SVSPRINTF( "%# 01.1Lg", " 1.e+01",   (long double) 9.8l);
    TEST_SVSPRINTF( "%#.1g",     "-4.e+04",   (double) -40661.5);
    TEST_SVSPRINTF( "%#.1Lg",    "-4.e+04",   (long double) -40661.5l);
    TEST_SVSPRINTF( "%#g",       "0.00000",   (double) 0.0);
    TEST_SVSPRINTF( "%#Lg",      "0.00000",   (long double) 0.0l);
    TEST_SVSPRINTF( "%g",        "0",         (double) 0.0);
    TEST_SVSPRINTF( "%Lg",       "0",         (long double) 0.0l);

    TEST_SVSPRINTF("%+5.10r2E %+10.10r2e %+10.10r7e %+10.10r4e %+10.0e %+10.0e ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
                    (double) 3.9 ARG(7.89456123e-307) ARG(7.89456123e+307) ARG(789456123.0) ARG(0.789456123) ARG(DBL_MIN));

    TEST_SVSPRINTF("%+5.10r2LE %+10.10r2Le %+10.10r7Le %+10.10r4Le %+10.0Le %+10.0Le ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
                    (long double) 3.9 ARG((long double) 7.89456123e-307) ARG((long double) 7.89456123e+307) ARG((long double) 789456123.0) ARG((long double) 0.789456123) ARG((long double) DBL_MIN));

    TEST_SVSPRINTF("%+5.10r*E %+10.10r*e %+10.10r*e %+10.10r*e %+10.0e %+10.0e ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
                    (int) 2 ARG(3.9) ARG((int) 2) ARG(7.89456123e-307) ARG((int) 7) ARG(7.89456123e+307) ARG((int) 4) ARG(789456123.0) ARG(0.789456123) ARG(DBL_MIN));

    TEST_SVSPRINTF("%+5.10r*LE %+10.10r*Le %+10.10r*Le %+10.10r*Le %+10.0Le %+10.0Le ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
                    (int) 2 ARG((long double) 3.9) ARG((int) 2) ARG((long double) 7.89456123e-307) ARG((int) 7) ARG((long double) 7.89456123e+307) ARG((int) 4) ARG((long double) 789456123.0) ARG((long double) 0.789456123) ARG((long double) DBL_MIN));

    TEST_SVSPRINTF( "%r0I16d",     "100",                       (int16_t) 100 );
    TEST_SVSPRINTF( "%r1I16d",     "64",                        (int16_t) 100 );
    TEST_SVSPRINTF( "%r*I16d",     "-1000",                     (int) 5 ARG((int16_t) -125));

    TEST_SVSPRINTF( "%r0I32d",     "100",                       (int32_t) 100 );
    TEST_SVSPRINTF( "%r1I32d",     "64",                        (int32_t) 100 );
    TEST_SVSPRINTF( "%r*I32d",     "-1000",                     (int) 5 ARG((int32_t) -125));

    TEST_SVSPRINTF( "%r0I64d",     "100",                       (int64_t) 100 );
    TEST_SVSPRINTF( "%r1I64d",     "64",                        (int64_t) 100 );
    TEST_SVSPRINTF( "%r*I64d",     "-1000",                     (int) 5 ARG((int64_t) -125));

    TEST_SVSPRINTF( "%r0I16u",     "100",                       (uint16_t) 100 );
    TEST_SVSPRINTF( "%r1I16u",     "64",                        (uint16_t) 100 );
    TEST_SVSPRINTF( "%r*I16u",     "1000",                      (int) 5 ARG((uint16_t) 125));

    TEST_SVSPRINTF( "%r0I32u",     "100",                       (uint32_t) 100 );
    TEST_SVSPRINTF( "%r1I32u",     "64",                        (uint32_t) 100 );
    TEST_SVSPRINTF( "%r*I32u",     "1000",                      (int) 5 ARG((uint32_t) 125));

    TEST_SVSPRINTF( "%r0I64u",     "100",                       (int64_t) 100 );
    TEST_SVSPRINTF( "%r1I64u",     "64",                        (int64_t) 100 );
    TEST_SVSPRINTF( "%r*I64u",     "1000",                      (int) 5 ARG((uint64_t) 125));

    TEST_SVSPRINTF( "%r0d",        "100",                       (int) 100 );
    TEST_SVSPRINTF( "%r1d",        "64",                        (int) 100 );
    TEST_SVSPRINTF( "%r*d",        "-1000",                     (int) 5 ARG((int) -125));

    TEST_SVSPRINTF( "%r0u",        "100",                       (unsigned int) 100 );
    TEST_SVSPRINTF( "%r1u",        "64",                        (unsigned int) 100 );
    TEST_SVSPRINTF( "%r*u"   ,     "1000",                      (int) 5 ARG((unsigned int) 125));

    TEST_SVSPRINTF( "%r0ld",       "100",                       (long) 100 );
    TEST_SVSPRINTF( "%r1ld",       "64",                        (long) 100 );
    TEST_SVSPRINTF( "%r*ld",       "-1000",                     (int) 5 ARG((long) -125));

    TEST_SVSPRINTF( "%r0lu",       "100",                       (unsigned long) 100 );
    TEST_SVSPRINTF( "%r1lu",       "64",                        (unsigned long) 100 );
    TEST_SVSPRINTF( "%r*lu",       "1000",                      (int) 5 ARG((unsigned long) 125));

    TEST_SVSPRINTF( "%r0lld",      "100",                       (long long) 100 );
    TEST_SVSPRINTF( "%r1lld",      "64",                        (long long) 100 );
    TEST_SVSPRINTF( "%r*lld",      "-1000",                     (int) 5 ARG((long long) -125));

    TEST_SVSPRINTF( "%r0llu",      "100",                       (unsigned long long) 100 );
    TEST_SVSPRINTF( "%r1llu",      "64",                        (unsigned long long) 100 );
    TEST_SVSPRINTF( "%r*llu",      "1000",                      (int) 5 ARG((unsigned long long) 125));

    TEST_SVSPRINTF( "%r0l1u",      "100",                       (uint8_t) 100 );
    TEST_SVSPRINTF( "%r1l1u",      "64",                        (uint8_t) 100 );
    TEST_SVSPRINTF( "%r*l1u",      "1000",                      (int) 5 ARG((uint8_t) 125));

    TEST_SVSPRINTF( "%r0l1d",      "100",                       (int8_t) 100 );
    TEST_SVSPRINTF( "%r1l1d",      "64",                        (int8_t) 100 );
    TEST_SVSPRINTF( "%r*l1d",      "-1000",                     (int) 5 ARG((int8_t) -125));

    TEST_SVSPRINTF( "%r0l2u",      "100",                       (uint16_t) 100 );
    TEST_SVSPRINTF( "%r1l2u",      "64",                        (uint16_t) 100 );
    TEST_SVSPRINTF( "%r*l2u",      "1000",                      (int) 5 ARG((uint16_t) 125));

    TEST_SVSPRINTF( "%r0l2d",      "100",                       (int16_t) 100 );
    TEST_SVSPRINTF( "%r1l2d",      "64",                        (int16_t) 100 );
    TEST_SVSPRINTF( "%r*l2d",      "-1000",                     (int) 5 ARG((int16_t) -125));

    TEST_SVSPRINTF( "%r0l4u",      "100",                       (uint32_t) 100 );
    TEST_SVSPRINTF( "%r1l4u",      "64",                        (uint32_t) 100 );
    TEST_SVSPRINTF( "%r*l4u",      "1000",                      (int) 5 ARG((uint32_t) 125));

    TEST_SVSPRINTF( "%r0l4d",      "100",                       (int32_t) 100 );
    TEST_SVSPRINTF( "%r1l4d",      "64",                        (int32_t) 100 );
    TEST_SVSPRINTF( "%r*l4d",      "-1000",                     (int) 5 ARG((int32_t) -125));

    TEST_SVSPRINTF( "%r0l8u",      "100",                       (uint64_t) 100 );
    TEST_SVSPRINTF( "%r1l8u",      "64",                        (uint64_t) 100 );
    TEST_SVSPRINTF( "%r*l8u",      "1000",                      (int) 5 ARG((uint64_t) 125));

    TEST_SVSPRINTF( "%r0l8d",      "100",                       (int64_t) 100 );
    TEST_SVSPRINTF( "%r1l8d",      "64",                        (int64_t) 100 );
    TEST_SVSPRINTF( "%r*l8d",      "-1000",                     (int) 5 ARG((int64_t) -125));
 
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
