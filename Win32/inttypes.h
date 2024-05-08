/*****************************************************************************\
*                                                                             *
*  FILE NAME:     inttypes.h                                                  *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  DESCRIPTION:   incomplete wrapper for inttypes.h for MSVC compilers        *
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


#ifndef INTTYPES_H
#define INTTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32

#undef int8_t
#undef uint8_t
#undef int16_t
#undef uint16_t
#undef int32_t
#undef uint32_t
#undef int64_t
#undef uint64_t
#undef int_fast8_t
#undef uint_fast8_t
#undef int_fast16_t
#undef uint_fast16_t
#undef int_fast32_t
#undef uint_fast32_t
#undef int_fast64_t
#undef uint_fast64_t
#undef int_least8_t
#undef uint_least8_t
#undef int_least16_t
#undef uint_least16_t
#undef int_least32_t
#undef uint_least32_t
#undef int_least64_t
#undef uint_least64_t

#undef intmax_t
#undef uintmax_t

typedef          __int8  int8_t;
typedef unsigned __int8  uint8_t;
typedef          __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef          __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef          __int64 int64_t;
typedef unsigned __int64 uint64_t;

typedef          __int8  int_fast8_t;
typedef unsigned __int8  uint_fast8_t;
typedef          __int16 int_fast16_t;
typedef unsigned __int16 uint_fast16_t;
typedef          __int32 int_fast32_t;
typedef unsigned __int32 uint_fast32_t;
typedef          __int64 int_fast64_t;
typedef unsigned __int64 uint_fast64_t;

typedef          __int8  int_least8_t;
typedef unsigned __int8  uint_least8_t;
typedef          __int16 int_least16_t;
typedef unsigned __int16 uint_least16_t;
typedef          __int32 int_least32_t;
typedef unsigned __int32 uint_least32_t;
typedef          __int64 int_least64_t;
typedef unsigned __int64 uint_least64_t;

typedef          __int64 intmax_t;
typedef unsigned __int64 uintmax_t;

#define int8_t   int8_t
#define uint8_t  uint8_t
#define int16_t  int16_t
#define uint16_t uint16_t
#define int32_t  int32_t
#define uint32_t uint32_t
#define int64_t  int64_t
#define uint64_t uint64_t

#define int_fast8_t   int_fast8_t
#define uint_fast8_t  uint_fast8_t
#define int_fast16_t  int_fast16_t
#define uint_fast16_t uint_fast16_t
#define int_fast32_t  int_fast32_t
#define uint_fast32_t uint_fast32_t
#define int_fast64_t  int_fast64_t
#define uint_fast64_t uint_fast64_t

#define int_least8_t   int_least8_t
#define uint_least8_t  uint_least8_t
#define int_least16_t  int_least16_t
#define uint_least16_t uint_least16_t
#define int_least32_t  int_least32_t
#define uint_least32_t uint_least32_t
#define int_least64_t  int_least64_t
#define uint_least64_t uint_least64_t

#define intmax_t  intmax_t
#define uintmax_t uintmax_t

#define UINTMAX_MAX (~(uint64_t)0)
#define INTMAX_MIN  ((int64_t) 1 << 63)
#define INTMAX_MAX  (~(INT64_MIN))

#define UINT64_MAX (~(uint64_t)0)
#define INT64_MIN  ((int64_t) 1 << 63)
#define INT64_MAX  (~(INT64_MIN))

#define UINT32_MAX ((uint32_t)0xffffffff)
#define INT32_MIN  ((int32_t) 0x80000000)
#define INT32_MAX  ((int32_t) 0x7fffffff)

#define UINT16_MAX ((uint16_t)0xffff)
#define INT16_MIN  ((int16_t) 0x8000)
#define INT16_MAX  ((int16_t) 0x7fff)

#define UINT8_MAX ((uint8_t)0xff)
#define INT8_MIN  ((int8_t) 0x80)
#define INT8_MAX  ((int8_t) 0x7f)

#else  /* !_WIN32 */

#include <stdint.h>

#endif

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif /* INTTYPES_H */

/* ========================================================================== *\
   END OF FILE
\* ========================================================================== */
