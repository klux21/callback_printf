# Changelog of klux21/callback_printf

## callback_printf_2.2.12 / 2026-07-20
 - possibly invalid errno in case of 32 byte blank padding corrected

## callback_printf_2.2.11 / 2026-07-15
 - initial support of UTF-16 surrogate pairs

## callback_printf_2.2.10 / 2026-07-10
 - CHANGELOG.md added
 - initial version of run_tests.sh
 - README.txt renamed to README.md

## callback_printf_2.2.9 / 2026-06-26
 - typos corrected

## callback_printf_2.2.8 / 2026-06-10
 - uses lower case letters for alternative integer bases now
 - rounding of %Lf values corrected

## callback_printf_2.2.7 / 2026-04-19
 - SIZE_MAX,  PTRDIFF_MAX, intptr_t and uintprtr_t int Win32/inttypes.h defined

## callback_printf_2.2.6 / 2026-04-06
 - clang va_copy issues fixed
 -  minor improvements of UTF-8 decoding
 
## callback_printf_2.2.5 / 2026-03-10
 - possibly unexpected setting of errno fixed

## callback_printf_2.2.4 / 2026-03-05
 - powi and powil are using faster integer arithmetics for small exponents now

## callback_printf_2.2.3 / 2026-03-03
 - support of %@ improved

## callback_printf_2.2.2 / 2026-02-26
 - first support of %@ for inserting an additional format string and va_list in a format string

## callback_printf_2.2.1 / 2026-02-01
 - initial tests of the not string terminating functions added
 - initial support of length modifier I8
 - initial support of length modifier w8, w16, w32, and w64 according to C 23 standard
 - initial support of I8n, w8n, w16n, w32n, and w64n

## callback_printf_2.2.0 / 2026-01-05
 - one byte too short length of the output of svsnprintfu corrected
 - svsprintf sets errno now
 - several wrappers for writing of unterminated strings added

## callback_printf_2.1.4 / 2026-01-04
 - svsnprintf and ssnprintf ensure a standard conform string termination now

## callback_printf_2.1.3 / 2025-12-30
 - slightly faster floating point output

## callback_printf_2.1.2 / 2025-12-20
 - initial VS2010 project
 - minor optimization

## callback_printf_2.1.1 / 2025-12-14
 - cares about more bits of a floating point mantissa now

## callback_printf_2.1.0 / 2025-12-14
 - usually faster integer arithmetic for the floating point mantissa added
 -  additional tests of long double added

## callback_printf_2.0.6 / 2025-12-14
 - minor Win32 issue corrected
 - additional tests of long double added

## callback_printf_2.0.5 / 2025-12-11
 - minor speed optimization of floating point output

## callback_printf_2.0.4 / 2025-12-07
 - support of the formats %l1s, %l2s and %l4s for wide character strings of deviant byte width added

## callback_printf_2.0.3 / 2025-12-05
 - supports %r*hh now
 - some testcases of numeric systems support of floats added

## callback_printf_2.0.2 / 2025-11-25
 - minor changes and optimization

## callback_printf_2.0.1 / 2025-11-21
 - optional prefixing of hexadecimal floating point numbers
 - optional prefixing of binary floats
 - test for optional prefixing of binary floats added

## callback_printf_2.0.0 / 2025-11-08
 - first support of options %v and %V by callback_printf

## callback_printf_1.1.15 / 2025-11-06
 - bugfix for possibly unexpected setting of errno to EINVAL

## callback_printf_1.1.14 / 2025-11-04
 - support of %P for printing pointers with uppercase hexadecimal letters added

## callback_printf_1.1.13 / 2025-10-17
 - vfprintf wrapper added
 - several minor adjustments

## callback_printf_1.1.12 / 2025-05-01
 - workaround for VC++ 2022 x86 compiler bug added
 - strict-aliasing rules warning of gcc fixed
 - integer output simplified a bit
 - accidentally deleted security check readded

## callback_printf_1.1.11 / 2025-04-11
 -  VC++ compiler warning fixed

## callback_printf_1.1.10 / 2025-03-22
 - invalid output for %g and %G and numbers between 100000 and 999999 fixed
 - additional test for %g and %G added

## callback_printf_1.1.9 / 2024-12-19
 - format character variable e0 renamed to fc
 - supports optional int size of 64 bits now

## callback_printf_1.1.8 / 2024-05-15
 - README.txt improved
 - initial support of the numeric base prefix r* for integer types added
 - tests for r* for several integer bases added

## callback_printf_1.1.7 / 2024-03-24
 - minor correction of unused code
 - defines renamed

## callback_printf_1.1.6 / 2024-02-24
 - special test cases added

## callback_printf_1.1.5 / 2024-02-20
 - initial sample of a fprintf wrapper
 - missing length checks added and precision for %c specifies a repeat count now
 - additional tests added
 - struct _basepow renamed to basepow_s because of MS debugger problem
 - sfprintf.c and sfprintf.h added to project

## callback_printf_1.1.4 / 2024-02-18
 - identifiers renamed for matching C standard

## callback_printf_1.1.3 / 2024-02-12
 - UnixTime renamed to unix_time

## callback_printf_1.1.2 / 2024-02-10
 - Visual Studio test project added

## callback_printf_1.1.1 / 2024-01-28
 - wrappers for inttypes.h and stdint.h for Win32 added
 - vStrWriteCallback renamed to vsnprintf_write_callback and improved

## callback_printf_1.1.0 / 2024-07-10
 - Civil Usage Public License, Version 1.1, January 2024

## callback_printf_1.0.11 / 2023-12-04
 - code better formatted

## callback_printf_1.0.10 / 2023-10-28
 - compiler warnings because of wrong casts corrected

## callback_printf_1.0.9 / 2023-09-12
 - readability of code of printing decimals improved
 - additional tests added

## callback_printf_1.0.8 / 2023-09-07
 - initial version of .gitignore
 - support of Microsoft specific prefix I (uppercase i) for size_t and ptrdiff_t
 - faster decimal output of big integer values

## callback_printf_1.0.7 / 2023-08-15
 - bench_vsprintf renamed to vsprintf_bench
 - README.txt better formatted

## callback_printf_1.0.6 / 2023-08-13
 - additional test for hexadecimal  printing of size_t and ptrdiff_t added

## callback_printf_1.0.5 / 2023-08-12
 - initial support of the prefix r* and the numeric bases up to 36

## callback_printf_1.0.4 / 2023-08-10
 - long double test with big exponent added

## callback_printf_1.0.3 / 2023-08-09
 - wrong output format of callback_printf for %a and %A format fixed

## callback_printf_1.0.2 / 2023-08-08
 - README.txt created
 - sequence of percent sign tests added
 - minor optimizations of floating point output
 - output rule for the %g format cares rounding of the value with %E
 - additional tests for %g format added
 - bench_vsprintf.c for speed and result comparison with vsnprintf and vsprintf

## callback_printf_1.0.1 / 2023-08-03
 - minor optimizations of floating point output

## callback_printf_1.0.0 / 2023-08-02
 - cbk_printf renamed to callback_printf

## cbk_printf_1.0.0 / 2023-08-01
 - initial version
 