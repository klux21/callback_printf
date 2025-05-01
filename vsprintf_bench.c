#if 0
rm -f ./_vsprintf_bench
cc -Wall -O3 -o _vsprintf_bench -I . vsprintf_bench.c sfprintf.c callback_printf.c
./_vsprintf_bench
exit $?
#endif

/*****************************************************************************\
*                                                                             *
*  FILENAME:      bench_vsprintf.c                                            *
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
*  ORIGIN:        https://github/klux21/callback_printf                       *
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

#if defined (_WIN32) || defined (__CYGWIN__)
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <callback_printf.h>
#include <sfprintf.h>

#if defined (_WIN32) || defined (__CYGWIN__)

static void (WINAPI * vGetSystemTimePreciseAsFileTime)(LPFILETIME lpSystemTimeAsFileTime);
static HMODULE hmKernel32Dll = (HMODULE) -1;

/* ------------------------------------------------------------------------- *\
   unix_time returns the Unix time in microsecond (time since 01/01/1970)
\* ------------------------------------------------------------------------- */
int64_t unix_time()
{
   int64_t iRet;
   FILETIME CurrentTime;

   if(vGetSystemTimePreciseAsFileTime)
   {
      vGetSystemTimePreciseAsFileTime(&CurrentTime);
   }
   else if(hmKernel32Dll == (HMODULE) -1)
   { /* 1rst call */
      hmKernel32Dll = LoadLibrary("Kernel32.dll");
      if(hmKernel32Dll)
         vGetSystemTimePreciseAsFileTime = (void (WINAPI * )(LPFILETIME)) GetProcAddress(hmKernel32Dll, "GetSystemTimePreciseAsFileTime");

      if(vGetSystemTimePreciseAsFileTime)
         vGetSystemTimePreciseAsFileTime(&CurrentTime);
      else
         GetSystemTimeAsFileTime(&CurrentTime);
   }
   else
   {
      GetSystemTimeAsFileTime(&CurrentTime);
   }

   iRet  = ((int64_t) CurrentTime.dwHighDateTime << 32);
   iRet += (int64_t)  CurrentTime.dwLowDateTime;
   iRet -= (int64_t)  116444736 * 1000000 * 1000; /* offset of Windows FileTime to start of Unix time */
   return (iRet / 10);
}/* int64_t unix_time() */

#else

int64_t unix_time()
{
   int64_t tRet;
   struct timeval tv;

   gettimeofday(&tv, NULL);

   tRet = (int64_t) tv.tv_sec;

   /* Try to turn the year 2038 problem into a year 2106 problem. */
   if((sizeof(time_t) <= 4) && (tv.tv_sec < 0))
      tRet += (int64_t) 0x80000000ul + (int64_t) 0x80000000ul;

   tRet *= 1000000ul;
   tRet += tv.tv_usec;
   return (tRet);
}/* int64_t unix_time() */
#endif


/* int test_vsprintf(const char * pout, const char * call, const char * pfmt, ...)  __PRINTF_LIKE_ARGS (3, 4); */

int test_vsprintf(const char * pout, const char * call, const char * pfmt, ...)
{
    int bRet = 1;
    size_t outlen = strlen(pout);
    va_list VarArgs;

    char buf[0x8000];
    char * pb = buf;

    struct _result
    {
       const char * name; /* function name */
       int64_t      ts;   /* start time */
       int64_t      te;   /* end time */
       char *       pb;   /* pointer to output buffer */
       size_t       ret;  /* returned size */
    } results[8];
    struct _result * pr = results;

#if defined (_WIN32) || defined (__CYGWIN__)
    size_t loops = vGetSystemTimePreciseAsFileTime ? 20000 : 200000;
    size_t div   = vGetSystemTimePreciseAsFileTime ? 2 : 20;
#else
    size_t loops = 20000;
    size_t div   = 2;
#endif

    size_t count = 0;

    memset(results, 0, sizeof(results));
    memset(buf, 0xfefefefe,  sizeof(buf));

    /* --------------------------------------------------------------------------- */
    count = loops;
    pr->name = "vsprintf";
    pr->pb   = pb;
    pr->ts   = unix_time();
    while(count--)
    {
       va_start(VarArgs, pfmt);
       pr->ret = vsprintf(pb, pfmt, VarArgs);
       va_end(VarArgs);
    }
    pr->te   = unix_time();
    pb += 1024;
    pr++;

    /* --------------------------------------------------------------------------- */
    count = loops;
    pr->name = "vsnprintf";
    pr->pb   = pb;
    pr->ts   = unix_time();
    while(count--)
    {
       va_start(VarArgs, pfmt);
       pr->ret = vsnprintf(pb, 1024, pfmt, VarArgs);
       va_end(VarArgs);
    }
    pr->te   = unix_time();
    pb += 1024;
    pr++;

    /* --------------------------------------------------------------------------- */
    count = loops;
    pr->name = "svsprintf";
    pr->pb   = pb;
    pr->ts   = unix_time();
    while(count--)
    {
       va_start(VarArgs, pfmt);
       pr->ret = svsprintf(pb, pfmt, VarArgs);
       va_end(VarArgs);
    }
    pr->te   = unix_time();
    pb += 1024;
    pr++;

    /* --------------------------------------------------------------------------- */
    count = loops;
    pr->name = "svsnprintf";
    pr->pb   = pb;
    pr->ts   = unix_time();
    while(count--)
    {
       va_start(VarArgs, pfmt);
       pr->ret = svsnprintf(pb, 1024, pfmt, VarArgs);
       va_end(VarArgs);
    }
    pr->te   = unix_time();
    pb += 1024;
    pr++;

    /* --------------------------------------------------------------------------- */

    printf("Call : %s\n", call);

    pr = results;
    while(pr->name)
    {
       int64_t tm   = pr->te - pr->ts;
       int     failed = (outlen != pr->ret) || strcmp(pout, pr->pb);
       sfprintf(stdout, "%10s:  %3ld.%.4ldus %s \"%s\"\n", pr->name, (long) (tm / loops), (long) ((tm % loops + (div / 2)) / div), failed ? "!NOK!" : "  OK ", pr->pb);
       if(failed)
       bRet = 0;
       ++pr;
    }

    printf("\n");
    return (bRet);
} /* int test_vsprintf(...) */


int run_tests()
{
   int bRet = 1;

   static volatile uint32_t inf  = 0x7f800000;
   static volatile uint32_t ninf = 0xff800000;
   static volatile uint32_t nan  = 0x7fc00000;
   static volatile uint32_t nnan = 0xffc00000;

#define ARG(x)    ,x

#define TEST_VSPRINTF(pfmt, pout, val) bRet &= test_vsprintf(pout, #pfmt ", " #val " : " #pout , pfmt, val);

    TEST_VSPRINTF( "Hallo Welt!",   "Hallo Welt!",               0);
    TEST_VSPRINTF( "%s",            "Hallo Welt!",              "Hallo Welt!");
    TEST_VSPRINTF( "%.10s!",        "Hallo Welt!",               "Hallo Welt");
    TEST_VSPRINTF( "%.*s!",         "Hallo Welt!",               (int) 10 ARG("Hallo Welt"));
    TEST_VSPRINTF( "%.5s %.5s",     "Hallo Welt!",               "Hallo" ARG("Welt!"));
    TEST_VSPRINTF( "%-6.5s%4.5s!",  "Hallo Welt!",               "Hallo" ARG("Welt"));
    TEST_VSPRINTF( "%c%c%c%c%c %c%c%c%c!", "Hallo Welt!", 'H' ARG('a') ARG('l') ARG('l') ARG('o') ARG('W') ARG('e') ARG('l') ARG('t'));
    TEST_VSPRINTF( "%02d/%02d/%04d %02d:%02d:%02d", "01/01/1970 23:59:59", 1 ARG(1) ARG(1970) ARG(23) ARG(59) ARG(59));
    TEST_VSPRINTF( "%.2d/%.2d/%.4d %.2d:%.2d:%.2d", "01/01/1970 23:59:59", 1 ARG(1) ARG(1970) ARG(23) ARG(59) ARG(59));
    TEST_VSPRINTF( "%+#23.15e",     " +1.000000000000000e-01",   (double) 1.0e-1 );
    TEST_VSPRINTF( "%+#23.15e",     " +3.900000000000000e+00",   (double) 3.9 );
    TEST_VSPRINTF( "%+#23.14e",     " +7.89456123000000e-307",   (double) 7.89456123e-307 );
    TEST_VSPRINTF( "%+#23.14e",     " +7.89456123000000e+307",   (double) 7.89456123e+307 );
    TEST_VSPRINTF( "%+#23.15e",     " +7.894561230000000e+08",   (double) 789456123.0 );
    TEST_VSPRINTF( "%-#23.15e",     "7.894561230000000e+08  ",   (double) 789456123.0 );
    TEST_VSPRINTF( "%#23.15e",      "  7.894561230000000e+08",   (double)789456123.0 );
    TEST_VSPRINTF( "%#1.1g",        "8.e+08",                    (double) 789456123.0 );
    TEST_VSPRINTF( "%+#23.15Le",    " +1.000000000000000e-01",   (long double) 1.0e-1l );
    TEST_VSPRINTF( "%+#23.15Le",    " +3.900000000000000e+00",   (long double) 3.9l );
    TEST_VSPRINTF( "%+#27.6e",    "             +1.594561e-317", (double) 1.59456123e-317 );
    TEST_VSPRINTF( "%+#27.16e",    "   +1.5945612300000000e+308", (double) 1.59456123e+308 );
#ifdef _WIN32
    TEST_VSPRINTF( "%+#27.15Le",    "   +9.89456123000000000e-307",(long double) 9.89456123e-307l );
    TEST_VSPRINTF( "%+#27.15Le",    "   +9.89456123000000000e+307",(long double) 9.89456123e+307l );
#else
    TEST_VSPRINTF( "%+#27.17Le",    " +7.89456123000000000e-4307",(long double) 7.89456123e-4307l );
    TEST_VSPRINTF( "%+#27.17Le",    " +7.89456123000000000e+4307",(long double) 7.89456123e+4307l );
#endif
    TEST_VSPRINTF( "%+#23.15Le",    " +7.894561230000000e+08",   (long double) 789456123.0l );
    TEST_VSPRINTF( "%-#23.15Le",    "7.894561230000000e+08  ",   (long double) 789456123.0l );
    TEST_VSPRINTF( "%#23.15Le",     "  7.894561230000000e+08",   (long double) 789456123.0l );
    TEST_VSPRINTF( "%#1.1Lg",       "8.e+08",                    (long double) 789456123.0l );
    TEST_VSPRINTF( "%lld",          "-8589934591",               (unsigned long long) ((unsigned long long)0xffffffff)*0xffffffff );
    TEST_VSPRINTF( "%+8lld",        "    +100",                  (unsigned long long) 100 );
    TEST_VSPRINTF( "%+.8lld",       "+00000100",                 (unsigned long long) 100 );
    TEST_VSPRINTF( "%+10.8lld",     " +00000100",                (unsigned long long) 100 );
    TEST_VSPRINTF( "%-1.5lld",      "-00100",                    (unsigned long long) -100 );
    TEST_VSPRINTF( "%5lld",         "  100",                     (unsigned long long) 100 );
    TEST_VSPRINTF( "%5lld",         " -100",                     (unsigned long long) -100 );
    TEST_VSPRINTF( "%-5lld",        "100  ",                     (unsigned long long) 100 );
    TEST_VSPRINTF( "%-5lld",        "-100 ",                     (unsigned long long) -100 );
    TEST_VSPRINTF( "%-.5lld",       "00100",                     (unsigned long long) 100 );
    TEST_VSPRINTF( "%-.5lld",       "-00100",                    (unsigned long long) -100 );
    TEST_VSPRINTF( "%-8.5lld",      "00100   ",                  (unsigned long long) 100 );
    TEST_VSPRINTF( "%-8.5lld",      "-00100  ",                  (unsigned long long) -100 );
    TEST_VSPRINTF( "%05lld",        "00100",                     (unsigned long long) 100 );
    TEST_VSPRINTF( "%05lld",        "-0100",                     (unsigned long long) -100 );
    TEST_VSPRINTF( "% lld",         " 100",                      (unsigned long long) 100 );
    TEST_VSPRINTF( "% lld",         "-100",                      (unsigned long long) -100 );
    TEST_VSPRINTF( "% 5lld",        "  100",                     (unsigned long long)  100 );
    TEST_VSPRINTF( "% 5lld",        " -100",                     (unsigned long long)  -100 );
    TEST_VSPRINTF( "% .5lld",       " 00100",                    (unsigned long long) 100 );
    TEST_VSPRINTF( "% .5lld",       "-00100",                    (unsigned long long) -100 );
    TEST_VSPRINTF( "% 8.5lld",      "   00100",                  (unsigned long long) 100 );
    TEST_VSPRINTF( "% 8.5lld",      "  -00100",                  (unsigned long long) -100 );
    TEST_VSPRINTF( "%.0lld",        "",                          (unsigned long long) 0 );
    TEST_VSPRINTF( "%8.0lld",       "        ",                  (unsigned long long) 0 );
    TEST_VSPRINTF( "%08.0lld",      "        ",                  (unsigned long long) 0 );
    TEST_VSPRINTF( "%#+21.18llx",   " 0x00ffffffffffffff9c",     (unsigned long long) -100 );
    TEST_VSPRINTF( "%#.25llo",      "0001777777777777777777634", (unsigned long long) -100 );
    TEST_VSPRINTF( "%#+24.20llo",   " 01777777777777777777634",  (unsigned long long) -100 );
    TEST_VSPRINTF( "%#+18.21llX",   "0X00000FFFFFFFFFFFFFF9C",   (unsigned long long) -100 );
    TEST_VSPRINTF( "%#+20.24llo",   "001777777777777777777634",  (unsigned long long) -100 );
    TEST_VSPRINTF( "%#+25.22llu",   "   0018446744073709551615", (unsigned long long) -1 );
    TEST_VSPRINTF( "%#+25.22llu",   "   0018446744073709551615", (unsigned long long) -1 );
    TEST_VSPRINTF( "%#+30.25llu",   "     0000018446744073709551615", (unsigned long long) -1 );
    TEST_VSPRINTF( "%+#25.22lld",   "  -0000000000000000000001", (unsigned long long) -1 );
    TEST_VSPRINTF( "%#-8.5llo",     "00144   ",                  (unsigned long long) 100 );
    TEST_VSPRINTF( "%#-+ 08.5lld",  "+00100  ",                  (unsigned long long) 100 );
    TEST_VSPRINTF( "%.80lld",       "00000000000000000000000000000000000000000000000000000000000000000000000000000001",  (unsigned long long) 1 );
    TEST_VSPRINTF( "% .80lld",      " 00000000000000000000000000000000000000000000000000000000000000000000000000000001", (unsigned long long) 1 );
    TEST_VSPRINTF( "% .80d",        " 00000000000000000000000000000000000000000000000000000000000000000000000000000001", (int) 1 );
    TEST_VSPRINTF( "%zx",           "1",              (size_t) 1 );
    TEST_VSPRINTF( "%zx",           sizeof(size_t) > 4 ? "ffffffffffffffff" : "ffffffff", (size_t) -1 );
    TEST_VSPRINTF( "%tx",           "1",              (ptrdiff_t) 1 );
    TEST_VSPRINTF( "%tx",           sizeof(size_t) > 4 ? "8000000000000000" : "80000000", (ptrdiff_t) 1 << (sizeof(ptrdiff_t) * 8 - 1));
    TEST_VSPRINTF( "% d",           " 1",             (int) 1 );
    TEST_VSPRINTF( "%+ d",          "+1",             (int) 1 );
    TEST_VSPRINTF( "%S",            "wide",           (wchar_t *) L"wide" );
    TEST_VSPRINTF( "%04c",          "   1",           (int) '1' );
    TEST_VSPRINTF( "%-04c",         "1   ",           (int) '1' );
    TEST_VSPRINTF( "%#012x",        "0x0000000001",   (int) 1 );
    TEST_VSPRINTF( "%#012x",        "000000000000",   (int) 0 );
    TEST_VSPRINTF( "%#04.8x",       "0x00000001",     (int) 1 );
    TEST_VSPRINTF( "%#04.8x",       "00000000",       (int) 0 );
    TEST_VSPRINTF( "%#-08.2x",      "0x01    ",       (int) 1 );
    TEST_VSPRINTF( "%#-08.2x",      "00      ",       (int) 0 );
    TEST_VSPRINTF( "%#.0x",         "0x1",            (int) 1 );
    TEST_VSPRINTF( "%#.0x",         "",               (int) 0 );
    TEST_VSPRINTF( "%#08o",         "00000001",       (int) 1 );
    TEST_VSPRINTF( "%#o",           "01",             (int) 1 );
    TEST_VSPRINTF( "%#o",           "0",              (int) 0 );
    TEST_VSPRINTF( "%04s",          " foo",           (char *) "foo" );
    TEST_VSPRINTF( "%.1s",          "f",              (char *) "foo" );
    TEST_VSPRINTF( "%.0s",          "",               (char *) "foo" );
    TEST_VSPRINTF( "hello",         "hello",          0 );
    TEST_VSPRINTF( "%b",            "101010",         (int) 42 );
    TEST_VSPRINTF( "%#b",           "0b101010",       (int) 42 );
    TEST_VSPRINTF( "%#B",           "0B101010",       (int) 42 );
    TEST_VSPRINTF( "%3c",           "  a",            (int) 'a' );
    TEST_VSPRINTF( "%-3c",          "a  ",            (int) 'a' );
    TEST_VSPRINTF( "%3d",           "1234",           (int) 1234 );
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
 //   TEST_VSPRINTF( "%s",            "<NULL>",         (char *) NULL );
    TEST_VSPRINTF( "%s",            "%%%%",           (char *) "%%%%" );
    TEST_VSPRINTF( "%u",            "4294967295",     (int) -1 );
    TEST_VSPRINTF( "%u",            "2147483648",     (unsigned int) 0x80000000 );
    TEST_VSPRINTF( "%d",            "-2147483648",    (int) 0x80000000 );
    TEST_VSPRINTF( "%jd",           "-1",             (intmax_t) -1 );
    TEST_VSPRINTF( "%F",            "0.000000",       (double) 0 );
    TEST_VSPRINTF( "x%cx",          "xXx",            (int) 0x100+'X' );
    TEST_VSPRINTF( "%%0",           "%0",             0 );
    TEST_VSPRINTF( "%%%%%%%%",      "%%%%",           0 );
    TEST_VSPRINTF( "%%%%%d%%%%",    "%%42%%",         (int) 42 );
    TEST_VSPRINTF( "%hx",           "2345",           (int) 0x12345 );
    TEST_VSPRINTF( "%hhx",          "23",             (int) 0x123 );
    TEST_VSPRINTF( "%hhx",          "45",             (int) 0x12345 );
    TEST_VSPRINTF( "%c",            "a",              (int) 'a' );
    TEST_VSPRINTF( "%c",            "\x82",           (int) 0xa082 );
//    TEST_VSPRINTF( "%C",            "a",              (int) 'a' );
//    TEST_VSPRINTF( "%C",            "\xe3\x81\x82" /*""*/, (int) 0x3042 );
//    TEST_VSPRINTF( "%l4c",          "\xe3\x81\x82" /*""*/, (int) 0x3042 );
//    TEST_VSPRINTF( "%l2c",          "\xe3\x81\x82" /*""*/, (int) 0x3042 );
    TEST_VSPRINTF( "a%Cb",          "a\xe3\x81\x82" "b",   (int) 0x3042 );
    TEST_VSPRINTF( "%lld",          "-8589934591",          (unsigned long long) ((unsigned long long)0xffffffff)*0xffffffff );
    TEST_VSPRINTF( "%llu",          "18446744065119617025", (unsigned long long) ((unsigned long long)0xffffffff)*0xffffffff );
    TEST_VSPRINTF( "%d",            "1",             (int) 1 );
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

    TEST_VSPRINTF( "%.3a",  "-0x1.a37p-10", (double)      -1.6e-3 );
    TEST_VSPRINTF( "%.3A",  "-0X1.A37P-10", (double)      -1.6e-3 );
    TEST_VSPRINTF( "%.3La", "-0x1.a37p-10", (long double) -1.6e-3 );
    TEST_VSPRINTF( "%.3LA", "-0X1.A37P-10", (long double) -1.6e-3 );

    TEST_VSPRINTF( "%5.2e%.0f",  "  inf3",   (double) *(float*) &inf ARG(3.0));
    TEST_VSPRINTF( "%5.2e%.0f",  " -inf3",   (double) *(float*) &ninf ARG(3.0));
    TEST_VSPRINTF( "%5.2e%.0f",  "  nan3",   (double) *(float*) &nan ARG(3.0));
    TEST_VSPRINTF( "%5.2e%.0f",  " -nan3",   (double) *(float*) &nnan ARG(3.0));
    TEST_VSPRINTF( "%5.2E%.0f",  "  INF-3",  (double) *(float*) &inf ARG(-3.0));
    TEST_VSPRINTF( "%5.2E%.0f",  " -INF-3",  (double) *(float*) &ninf ARG(-3.0));
    TEST_VSPRINTF( "%5.2E%.0f",  "  NAN-3",  (double) *(float*) &nan ARG(-3.0));
    TEST_VSPRINTF( "%5.2E%.0f",  " -NAN-3",  (double) *(float*) &nnan ARG(-3.0));

    TEST_VSPRINTF( "%5.2Le%.0Lf", "  inf3",  (long double) *(float*) &inf ARG((long double) 3.0));
    TEST_VSPRINTF( "%5.2Le%.0Lf", " -inf3",  (long double) *(float*) &ninf ARG((long double) 3.0));
    TEST_VSPRINTF( "%5.2Le%.0Lf", "  nan3",  (long double) *(float*) &nan ARG((long double) 3.0));
    TEST_VSPRINTF( "%5.2Le%.0Lf", " -nan3",  (long double) *(float*) &nnan ARG((long double) 3.0));
    TEST_VSPRINTF( "%5.2LE%.0Lf", "  INF-3", (long double) *(float*) &inf ARG((long double) -3.0));
    TEST_VSPRINTF( "%5.2LE%.0Lf", " -INF-3", (long double) *(float*) &ninf ARG((long double) -3.0));
    TEST_VSPRINTF( "%5.2LE%.0Lf", "  NAN-3", (long double) *(float*) &nan ARG((long double) -3.0));
    TEST_VSPRINTF( "%5.2LE%.0Lf", " -NAN-3", (long double) *(float*) &nnan ARG((long double) -3.0));

    TEST_VSPRINTF( "%# 01.1g",  " 1.e+01",   (double) 9.8);
    TEST_VSPRINTF( "%# 01.1Lg", " 1.e+01",   (long double) 9.8l);
    TEST_VSPRINTF( "%#.1g",     "-4.e+04",   (double) -40661.5);
    TEST_VSPRINTF( "%#.1Lg",    "-4.e+04",   (long double) -40661.5l);
    TEST_VSPRINTF( "%#g",       "0.00000",   (double) 0.0);
    TEST_VSPRINTF( "%#Lg",      "0.00000",   (long double) 0.0l);
    TEST_VSPRINTF( "%g",        "0",         (double) 0.0);
    TEST_VSPRINTF( "%Lg",       "0",         (long double) 0.0l);
    TEST_VSPRINTF( "%g",        "490000",    (double) 490000.0l);
    TEST_VSPRINTF( "%Lg",       "490000",    (long double) 490000.0l);
    TEST_VSPRINTF( "%G",        "4.9E+06",   (double) 4900000.0l);
    TEST_VSPRINTF( "%LG",       "4.9E+06",   (long double) 4900000.0l);
    TEST_VSPRINTF( "%.7G",      "4900000",   (double) 4900000.0l);
    TEST_VSPRINTF( "%.7LG",     "4900000",   (long double) 4900000.0l);

//    TEST_VSPRINTF("%+5.10r2E %+10.10r2e %+10.10r7e %+10.10r4e %+10.0e %+10.0e ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
//                    (double) 3.9 ARG(7.89456123e-307) ARG(7.89456123e+307) ARG(789456123.0) ARG(0.789456123) ARG(DBL_MIN));

//    TEST_VSPRINTF("%+5.10r2LE %+10.10r2Le %+10.10r7Le %+10.10r4Le %+10.0Le %+10.0Le ", "+1.1111001101E+01 +1.0001101111e-1111111001 +1.6250223006e+1030 +2.3300320211e+32     +8e-01    +2e-308 ",
//                    (long double) 3.9 ARG((long double) 7.89456123e-307) ARG((long double) 7.89456123e+307) ARG((long double) 789456123.0) ARG((long double) 0.789456123) ARG((long double) DBL_MIN));

    return (bRet);
} /* run_tests() */


/* ------------------------------------------------------------------------- *\
   main function
\* ------------------------------------------------------------------------- */
int main(int argc, char * argv[])
{
    int iRet = 1;

    if(!run_tests())
        goto Exit;

    iRet = 0;

    Exit:;

#if 0
    if(!iRet)
        sfprintf(stdout, "All tests passed!\n");
    else
        sfprintf(stderr, "Tests failed!\n");
#endif

    return (iRet);
}/* main() */


/* ========================================================================= *\
   END OF FILE
\* ========================================================================= */
