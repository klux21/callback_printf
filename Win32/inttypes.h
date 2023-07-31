/*****************************************************************************\
*                                                                             *
*  FILE NAME   :  inttypes.h                                                  *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  DESCRIPTION :  incomplete wrapper for inttypes.h for MSVC compilers        *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  COPYRIGHT   : (c) 2023 Dipl.-Ing. Klaus Lux (Aachen, Germany)              *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  As long as there is no other contract you are allowed to use this source   *
*  code according to the following conditions:                                *
*                                                                             *
*  This program is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU General Public License as published by       *
*  the Free Software Foundation, either version 3 of the License.             *
*                                                                             *
*  This program is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of             *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
*  GNU General Public License for more details.                               *
*                                                                             *
*  You should have received a copy of the GNU General Public License          *
*  along with this program. If not, see <http://www.gnu.org/licenses/>.       *
*                                                                             *
*                                                                             *
*  Sofern keine anderen Vereinbarungen getroffen wurden, ist es Ihnen         *
*  erlaubt diesen Sourcecode folgenden Bedingungen zu verwenden:              *
*                                                                             *
*  Dieses Programm ist Freie Software: Sie können es unter den Bedingungen    *
*  der GNU General Public License, wie von der Free Software Foundation,      *
*  Version 3 der Lizenz weiter verteilen und/oder modifizieren.               *
*                                                                             *
*  Dieses Programm wird in der Hoffnung bereitgestellt, dass es nützlich      *
*  sein wird, jedoch OHNE JEDE GEWÄHR,; sogar ohne die implizite              *
*  Gewähr der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.         *
*  Siehe die GNU General Public License für weitere Einzelheiten.             *
*                                                                             *
*  Sie sollten eine Kopie der GNU General Public License zusammen mit         *
*  diesem Programm erhalten haben.                                            *
*  Wenn nicht, siehe <https://www.gnu.org/licenses/>.                         *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  REPOSITORY  :  https://github/klux21/cbk_printf                            *
*                                                                             *
\*****************************************************************************/

#ifndef __INTTYPES_H__
#define __INTTYPES_H__

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

#endif /* __INTTYPES_H__ */

/* ========================================================================== *\
   END OF FILE
\* ========================================================================== */
