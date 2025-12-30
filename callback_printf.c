/*****************************************************************************\
*                                                                             *
*  FILE NAME:     callback_printf.c                                           *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  DESCRIPTION:   system independent (v)s(n)printf wrapper functions          *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  COPYRIGHT:     (c) 2025 Dipl.-Ing. Klaus Lux (Aachen, Germany)             *
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
* --------------------------------------------------------------------------- *
*                                                                             *
* What are the advantages of using this implementation instead of using the   *
* build-in functions of your compiler?                                        *
*                                                                             *
* Security:    You do not share your possibly private internal string data    *
*              with 3rd party compiler libraries which are easy to modify.    *
* Portability: The implementation behaves the same on every plattform as      *
*              long as the data types have the same sizes. You do not have    *
*              to worry about different format strings on every platform.     *
* Features:    This implementation is pretty fast and provides some special   *
*              features as the common length modiers for %n, output of        *
*              integers in binary format and a fully featured floating point  *
*              output in tridynal, quadral, hexal, heptal, octal and nonal    *
*              system. For selecting a special numeric base just add a        *
*              r2 .. r9 prefix to common integer or floation point format     *
*              specifiers. It's possible to speficy the numeric base using r* *
*              prefix and and additional integer argument that contains the   *
*              base, e.g for printing a long int using numeric base 4         *
*                ssprintf(buffer, "%r*ld", (int) 4, LONG_MIN);                *
*              callback_printf supports the Windows I64 and I32 integer       *
*              length modifiers as well as the additional modifiers l1, l2,   *
*              l4 and l8 for 8, 16, 32 and 64 bit wide integer arguments.     *
*              Wide characters and wide character string are always converted *
*              to UTF-8 output. You can use l1, l2 and l4 length modiefiers   *
*              for converting string of 8, 16 or 32 bit wide character size   *
*              on every platform.                                             *
*              And last but not least is it pretty ease to create your own    *
*              fully featured and fast sprintf compatible string generator    *
*              function by using callback_printf whithout worrying about      *
*              portability. This software is more than a toy.                 *
* Limits:      The function rebase uses some multiplications for calculating  *
*              the decimal mantissa and is able to convert values for more    *
*              base systems than just the decimal system. The conversion is   *
*              very fast but the math of the convertion costs a little bit of *
*              precision because of some additionally required floating point *
*              operations. This should be rarely a big problem of course.     *
*                                                                             *
*              The output of very big values with %f happens in %e format.    *
*              The output length of the mantissa of floating points is        *
*              limited in size.                                               *
*              callback_printf does not care about any differences of the     *
*              users locale.                                                  *
*                                                                             *
\*****************************************************************************/


#undef   _CRT_ERRNO_DEFINED
#include <errno.h>
#include <limits.h>
#include <float.h> /* LDBL_MAX */

#include <callback_printf.h>

#ifdef _WIN32
#pragma warning(disable : 4100 4127 4706 4710)
#endif

/* ========================================================================= *\
\* ========================================================================= */

const char * LowerDigit = "0123456789abcdefghijklmnopqrstuvwxyz"; /* array for encoding numbers */
const char * UpperDigit = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; /* array for encoding numbers */

/* Array of character type flags:
   0x01 : Characters: A-Z a-z _
   0x02 : Digits: 0 - 9
   0x04 : blanks and control characters
   0x08 : operators: \n \r ! " # $ % & ( ) * + , - . /  : ; < = > ? [ \ ] ^ @ { | } ~
   0x10 : printf format termination characters bBdiouxXaAeEfFgGsScCpPnvV% '\0'
   0x20 : printf format flag characters -+# 0
   0x40 : printf integer format characters bBdiouxX */

const uint8_t CharType[256] = { 0x10,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,   0x04,0x04,0x08,0x04, 0x04,0x08,0x04,0x04,
                                0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,   0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,

                             /*       !    "    #     $    %    &    '       (    )    *    +     ,    -    .    /   */
                                0x24,0x08,0x08,0x28, 0x08,0x18,0x08,0x08,   0x08,0x08,0x08,0x28, 0x08,0x28,0x08,0x08,
                             /*  0    1    2    3     4    5    6    7       8    9    :    ;     <    =    >    ?   */
                                0x22,0x02,0x02,0x02, 0x02,0x02,0x02,0x02,   0x02,0x02,0x08,0x08, 0x08,0x08,0x08,0x08,
                             /*  @    A    B    C     D    E    F    G       H    I    J    K     L    M    N    O   */
                                0x08,0x11,0x51,0x11, 0x01,0x91,0x91,0x91,   0x01,0x01,0x01,0x01, 0x01,0x01,0x01,0x01,
                             /*  P    Q    R    S     T    U    V    W       X    Y    Z    [     \    ]    ^    _   */
                                0x11,0x01,0x01,0x11, 0x01,0x01,0x11,0x01,   0x51,0x01,0x01,0x08, 0x08,0x08,0x08,0x01,
                             /*  `    a    b    c     d    e    f    g       h    i    j    k     l    m    n    o   */
                                0x08,0x11,0x51,0x11, 0x51,0x91,0x91,0x91,   0x01,0x51,0x01,0x01, 0x01,0x01,0x11,0x51,
                             /*  p    q    r    s     t    u    v    w       x    y    z    {     |    }    ~        */
                                0x11,0x01,0x01,0x11, 0x01,0x51,0x11,0x01,   0x51,0x01,0x01,0x08, 0x08,0x08,0x08,0x04,

                                0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,   0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,   0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,   0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,   0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,

                                0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,   0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,   0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,   0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,   0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x04};


/* ========================================================================= *\
   UTF8 encoding and decoding
\* ========================================================================= */

/* ------------------------------------------------------------------------- *\
   iUtf8Decode decodes an UTF8 data buffer. It stops at end of decodable
   source or if end of destination buffer gets reached. Source and
   destination buffer must not overlap. Source and destination pointer get
   moved behind last decoded character of converted input and output.
   The function returns -1 in case of wrong arguments and -2 in case of
   (currently ignored!) errors within source data. In success case 0 gets
   returned.
   If pDst is NULL then no output happens and pDstSize will be set to the
   required size of the destination buffer to hold the whole converted input
   and the source size and source pointer are unchanged.
\* ------------------------------------------------------------------------- */

int iUtf8Decode (void **  pDst,         /* destination buffer */
                 size_t * pDstSize,     /* remaining destination buffer size */
                 void **  pSrc,         /* source buffer */
                 size_t * pSrcSize,     /* remaining source data size */
                 size_t   DstCharWidth) /* width of destination characters */
{
   static uint8_t Utf8length[]={ 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,   1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,   1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,   1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,   1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
                                 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   /* 80 - 9f */
                                 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   /* a0 - bf */
                                 2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,   2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,   /* c0 - df */
                                 3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,   4,4,4,4,4,4,4,4, 5,5,5,5,6,6,7,2 }; /* e0 - ff */

   int err = 0; /* return value */
   size_t dz      = 0;
   size_t sz      = 0;
   size_t val     = 0x87654321;
   size_t max_val = 0;
   unsigned char * pv    = (unsigned char *) &val;
   unsigned char * ps;
   size_t l;

   int little_endian     = 0x21 == *pv; /* nonzero in case of little endian */

   if((pDst && !*pDst) || !pDstSize || (pDst && !*pDstSize) ||
      !pSrc || !*pSrc || !pSrcSize || !*pSrcSize ||
      !DstCharWidth || (DstCharWidth > sizeof(val)))
   {
      err = -1;
      goto Exit;
   }

   if(sizeof(val) != DstCharWidth)
      max_val = ~max_val >> ((sizeof(val) - DstCharWidth) << 3);
   else
      max_val = ~max_val;

   if(!little_endian)
      pv += sizeof(val);

   sz = *pSrcSize;
   ps = (unsigned char *) *pSrc;

   if(!pDst)
   {
      while(sz)
      {
         l = Utf8length[*ps];

         if(1 == l)
         {
            val = *ps;
            ps++;
            sz--;
         }
         else
         {
            if(!l)
            {/* Invalid character, skip this possibly a broken sequence. */
               sz--;
               ps++;
               err = -2;
               continue;
            }

            if(sz < l)
               break; /* buffer size smaller than character size */

            if(0xff == *ps)
            {/* For now we'll ignore 0xff and any immediately following character */
               ps += 2;
               sz -= 2;
               continue;
            }

            val = *ps++ & ~(0xff << (7-l));
            sz--;

            while(--l)
            {
               if(Utf8length[*ps])
                   break; /* ignore invalid encoded characters */

               val <<= 6;
               val += *ps++ & 0x3f;
               sz--;
            }

            if(l)
            { /* Let's skip invalid encoded characters and set error flag */
               err = -2;
               continue;
            }
         }

         if(val > max_val)
         {/* The decoded value exceeds character width. Let's replace it by the max. possible value. */
            err = -2;
            val = max_val;
         }

         dz += DstCharWidth;
      }
   }
   else
   {
      unsigned char * pd = (unsigned char *) *pDst;
      dz = *pDstSize;

      while(sz && (dz >= DstCharWidth))
      {
         l = Utf8length[*ps];

         if(1 == l)
         {
            val = *ps;
            ps++;
            sz--;
         }
         else
         {
            if(!l)
            {/* Invalid character, skip this possibly a broken sequence. */
               sz--;
               ps++;
               err = -2;
               continue;
            }

            if(sz < l)
               break; /* buffer size smaller than character size */

            if(0xff == *ps)
            {/* For now we'll ignore 0xff and any immediately following character */
               ps += 2;
               sz -= 2;
               continue;
            }

            val = *ps++ & ~(0xff << (7-l));
            sz--;

            while(--l)
            {
               if(Utf8length[*ps])
                   break; /* ignore invalid encoded characters */

               val <<= 6;
               val += *ps++ & 0x3f;
               sz--;
            }

            if(l)
            { /* Let's skip invalid encoded characters and set error flag */
               err = -2;
               continue;
            }
         }

         if(val > max_val)
         {/* The decoded value exceeds character width. Let's replace it by the max. possible value. */
            err = -2;
            val = max_val;
         }

         if(!pDst)
         {
            dz += DstCharWidth;
         }
         else
         {
            if(little_endian)
            {
               l = 0;
               while(l < DstCharWidth)
               {
                  pd[l] = pv[l];
                  l++;
               }
            }
            else
            {
               l = DstCharWidth;
               while(l)
               {
                  *(pd + DstCharWidth - l) = *(pv - l);
                  l--;
               }
            }

            pd += DstCharWidth;
            dz -= DstCharWidth;
         }
      }
      *pDst     = pd;
      *pSrc     = ps;
      *pSrcSize = sz;
   }

   *pDstSize = dz;

   Exit:;
   return (err);
}/* int iUtf8Decode (...) */



/* ------------------------------------------------------------------------- *\
   iUtf8Encode encodes unicode characters in UTF8 format. It stops at end of
   decodable source or if end of destination buffer gets reached. Source and
   destination buffer must not overlap. Source and destination pointer get
   moved behind last decoded character of converted input and output.
   The function returns -1 in case of wrong arguments and -2 in case of
   (currently without any output ignored!) errors within source data
   (e.g. if a source value exceeds 32 bit). In success case 0 gets returned.
   If pDst is NULL then no output happens and pDstSize will be set to the
   required size of the destination buffer to hold the whole converted input
   and the source size and source pointer are unchanged.
\* ------------------------------------------------------------------------- */

int iUtf8Encode (void **  pDst,         /* destination buffer */
                 size_t * pDstSize,     /* remaining destination buffer size */
                 void **  pSrc,         /* source buffer */
                 size_t * pSrcSize,     /* remaining source data size */
                 size_t   SrcCharWidth) /* width of source characters */
{
   int err = 0; /* return value */
   size_t dz   = 0;
   size_t sz   = 0;
   size_t val  = 0x87654321;
   unsigned char * pv = (unsigned char *) &val;
   unsigned char * ps;

   size_t cw   = 0;
   int little_endian  = 0x21 == *pv;

   if((pDst && !*pDst) || !pDstSize || (pDst && !*pDstSize) ||
      !pSrc || !*pSrc || !pSrcSize || !*pSrcSize ||
      (SrcCharWidth > sizeof(val)))
   {
      err = -1;
      goto Exit;
   }

   if(!little_endian)
      pv += sizeof(val);

   sz = *pSrcSize;
   ps = (unsigned char *) *pSrc;

   if (!pDst)
   {
      while(sz >= SrcCharWidth)
      {
         val = 0;

         if(little_endian)
         {
            cw = 0;
            while(cw < SrcCharWidth)
            {
               pv[cw] = ps[cw];
               cw++;
            }
         }
         else
         {
            cw = SrcCharWidth;
            while(cw)
            {
               *(pv-cw) = *(ps + SrcCharWidth - cw);
               cw--;
            }
         }

         if(val <= 0x7f)
            dz++;
         else if(val <= 0x7ff)
            dz += 2;
         else if(val <= 0xffff)
            dz += 3;
         else if(val <= 0x1fffff)
            dz += 4;
         else if(val <= 0x3ffffff)
            dz += 5;
         else if(val <= 0x7fffffff)
            dz += 6;
         else if(val <= 0xffffffff)
            dz += 7;
         else
         { /* We don't handle values that exceed 32 bits */
            err = -2;
         }

         ps += SrcCharWidth;
         sz -= SrcCharWidth;
      }
   }
   else
   {
      unsigned char * pd = (unsigned char *) *pDst;
      dz = *pDstSize;

      while(sz >= SrcCharWidth)
      {
         val = 0;

         if(little_endian)
         {
            cw = 0;
            while(cw < SrcCharWidth)
            {
               pv[cw] = ps[cw];
               cw++;
            }
         }
         else
         {
            cw = SrcCharWidth;
            while(cw)
            {
               *(pv-cw) = *(ps + SrcCharWidth - cw);
               cw--;
            }
         }

         if(val <= 0x7f)
         {
            if(dz < 1)
               break;
            dz--;

            *pd++ = (unsigned char) val;
         }
         else if(val <= 0x7ff)
         {
            if (dz < 2)
               break;
            dz -= 2;

            pd[0] = (unsigned char) (((val >> 6) & 0x1f) + 0xc0);
            pd[1] = (unsigned char) ((val & 0x3f)        + 0x80);
            pd += 2;
         }
         else if(val <= 0xffff)
         {
            if (dz < 3)
               break;
            dz -= 3;

            pd[0] = (unsigned char) (((val >> 12) & 0xf)  + 0xe0);
            pd[1] = (unsigned char) (((val >> 6)  & 0x3f) + 0x80);
            pd[2] = (unsigned char) ((val & 0x3f)         + 0x80);
            pd += 3;
         }
         else if(val <= 0x1fffff)
         {
            if (dz < 4)
               break;
            dz -= 4;

            pd[0] = (unsigned char) (((val >> 18) & 0x7)  + 0xf0);
            pd[1] = (unsigned char) (((val >> 12) & 0x3f) + 0x80);
            pd[2] = (unsigned char) (((val >> 6)  & 0x3f) + 0x80);
            pd[3] = (unsigned char) ((val & 0x3f)         + 0x80);
            pd += 4;
         }
         else if(val <= 0x3ffffff)
         {
            if (dz < 5)
               break;
            dz -= 5;

            pd[0] = (unsigned char) (((val >> 24) & 0x3)  + 0xf8);
            pd[1] = (unsigned char) (((val >> 18) & 0x3f) + 0x80);
            pd[2] = (unsigned char) (((val >> 12) & 0x3f) + 0x80);
            pd[3] = (unsigned char) (((val >> 6)  & 0x3f) + 0x80);
            pd[4] = (unsigned char) ((val & 0x3f)         + 0x80);
            pd += 5;
         }
         else if(val <= 0x7fffffff)
         {
            if(dz < 6)
               break;
            dz -= 6;

            pd[0] = (unsigned char) (((val >> 30) & 0x1)  + 0xfc);
            pd[1] = (unsigned char) (((val >> 24) & 0x3f) + 0x80);
            pd[2] = (unsigned char) (((val >> 18) & 0x3f) + 0x80);
            pd[3] = (unsigned char) (((val >> 12) & 0x3f) + 0x80);
            pd[4] = (unsigned char) (((val >> 6)  & 0x3f) + 0x80);
            pd[5] = (unsigned char) ((val & 0x3f)         + 0x80);
            pd += 6;
         }
         else if(val <= 0xffffffff)
         {
            if(dz < 7)
                break;
            dz -= 7;

            pd[0] = 0xfe;
            pd[1] = (unsigned char) (((val >> 30) &  0x3) + 0x80);
            pd[2] = (unsigned char) (((val >> 24) & 0x3f) + 0x80);
            pd[3] = (unsigned char) (((val >> 18) & 0x3f) + 0x80);
            pd[4] = (unsigned char) (((val >> 12) & 0x3f) + 0x80);
            pd[5] = (unsigned char) (((val >> 6)  & 0x3f) + 0x80);
            pd[6] = (unsigned char) ((val & 0x3f)         + 0x80);
            pd += 7;
         }
         else
         { /* We don't handle values that exceed 32 bits */
            err = -2;
         }

         ps += SrcCharWidth;
         sz -= SrcCharWidth;
      }

      *pDst     = pd; /* store the end position of the scan */
      *pSrc     = ps;
      *pSrcSize = sz;
   }

   *pDstSize = dz;

   Exit:;
   return (err);
} /* int iUtf8Encode (...) */



/* ========================================================================= *\
   callback_printf related stuff ...
\* ========================================================================= */

static const char * pzeros  = "00000000000000000000000000000000"; /* blanks to copy */
static const char * pblanks = "                                "; /* blanks to copy */
static const char * lower_digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char * upper_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";


static size_t cbk_print_number(void *            pUserData,      /* user specific context for the callback */
                               PRINTF_CALLBACK * pCB,            /* data write callback */
                               const char *      pvalue,         /* number string to to write out */
                               size_t            length,         /* length of number string */
                               const char *      padding,        /* pointer to padding bytes to fill in after prefix before the numeric value */
                               char              sign_char,      /* optional sign character to fill in */
                               uint8_t           prefix_base,    /* if nonzero base for C style prefixing of octals and hexadecimals if prefixeng is required required */
                               uint8_t           uppercase,      /* whether to use uppercase letters for prefix */
                               uint8_t           left_justified, /* left justified value witin the output data field */
                               size_t            minwidth,       /* minimum width of value to print */
                               size_t            fieldwidth)     /* minimum field width */
{
    size_t zRet = 0;
    char   prefix[8];
    char * pf = prefix;
    size_t prelen;  /* length of sign and prefix */

    /* ------------------------------------------------------------------------- *\
       prepare prefix value
    \* ------------------------------------------------------------------------- */

    if (sign_char)
       *pf++ = sign_char;

    if(prefix_base)
    {
       if(prefix_base == 16)
       {
          *pf++ = '0';
          *pf++ = uppercase ? 'X' : 'x';
       }
       else if (prefix_base == 8)
       {
          if(length >= minwidth)
             *pf++ = '0';
       }
       else if (prefix_base == 2)
       {
          *pf++ = '0';
          *pf++ = uppercase ? 'B' : 'b';
       }
    }

    prelen = pf - prefix;

    /* ------------------------------------------------------------------------- *\
       generate the output
    \* ------------------------------------------------------------------------- */

    if(minwidth > length)
        minwidth -= length;
    else
        minwidth = 0;

    if(fieldwidth <= (minwidth + length + prelen))
    {
       zRet = minwidth + length + prelen;

       if(prelen)
          pCB(pUserData, prefix, prelen);

       if(minwidth)
       {
          while (minwidth > 32) /* adjust the byte length 'x' to be swapped within the output string */
          {
             pCB(pUserData, pzeros, 32);
             minwidth -= 32;
          }
          pCB(pUserData, pzeros, minwidth);
       }

       if(length)
          pCB(pUserData, pvalue, length); /* output of value at begin */
    }
    else
    { /* we have to add filler characters for ensuring the minimum field width */
       zRet = fieldwidth;

       fieldwidth -= length + minwidth + prelen; /* calculate number of required fill characters */

       if(prelen && (padding == pzeros))
       {
          pCB(pUserData, prefix, prelen);
          prelen = 0;
       }

       if (!left_justified)
       { /* fill in leading blanks */
          while (fieldwidth > 32)
          {
             pCB(pUserData, padding, 32);
             fieldwidth -= 32;
          }
          pCB(pUserData, padding, fieldwidth);
       }

       if(prelen)
          pCB(pUserData, prefix, prelen);

       if(minwidth)
       { /* fill in leading zeros */
          while (minwidth > 32)
          {
             pCB(pUserData, pzeros, 32);
             minwidth -= 32;
          }
          pCB(pUserData, pzeros, minwidth);
       }

       if(length)
          pCB(pUserData, pvalue, length); /* output of value at begin */

       if (left_justified)
       { /* fill in trailing blanks */
          while (fieldwidth > 32)
          {
             pCB(pUserData, pblanks, 32);
             fieldwidth -= 32;
          }
          pCB(pUserData, pblanks, fieldwidth);
       }
    }

    return (zRet);
} /* size_t cbk_print_number(...) */



/* Array of the numeric bases that are related to the printf integer format characters */
static const uint8_t PrintfIntBase[256] = {10,16, 2, 3,  4, 5, 6, 7,   8, 9,10,11, 12,13,14,15,  16,17,18,19, 20,21,22,23,  24,25,26,27, 28,29,30,31,
                                         /*    !  "  #   $  %  &  '    (  )  *  +   ,  -  .  /    0  1  2  3   4  5  6  7    8  9  :  ;   <  =  >  ? */
                                           32,33,34,35, 36, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,  10,16, 2, 3,  4, 5, 6, 7,   8, 9, 0, 0,  0, 0, 0, 0,
                                         /* @  A  B  C   D  E  F  G    H  I  J  K   L  M  N  O    P  Q  R  S   T  U  V  W    X  Y  Z  [   \  ]  ^  _ */
                                            0, 0, 2, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,  16, 0, 0, 0,  0, 0, 0, 0,  16, 0, 0, 0,  0, 0, 0, 0,
                                         /* `  a  b  c   d  e  f  g    h  i  j  k   l  m  n  o    p  q  r  s   t  u  v  w    x  y  z  {   |  }  ~    */
                                            0, 0, 2, 0, 10, 0, 0, 0,   0,10, 0, 0,  0, 0, 0, 8,  16, 0, 0, 0,  0,10, 0, 0,  16, 0, 0, 0,  0, 0, 0, 0,
                                            0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                                            0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                                            0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                                            0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0 };

/* d00 contains the decimal numbers from 00 to 99 */
static const char * d00 = "00010203040506070809" "10111213141516171819" "20212223242526272829" "30313233343536373839" "40414243444546474849"
                          "50515253545556575859" "60616263646566676869" "70717273747576777879" "80818283848586878889" "90919293949596979899";

/* ------------------------------------------------------------------------- *\
   cbk_print_u64 prints an uint64_t using a printf callback function
   and returns the written string data length.
\* ------------------------------------------------------------------------- */

static size_t cbk_print_u64(void *            pUserData,      /* user specific context for the callback */
                            PRINTF_CALLBACK * pCB,            /* data write callback */
                            uint64_t          value,          /* value to write to the string */
                            char              format,         /* format character in the format string */
                            char              sign_char,      /* expected sign character */
                            uint8_t           prefixing,      /* C style prefixing of octals and hexadecimals required */
                            uint8_t           left_justified, /* left justified value witin the output data field */
                            uint8_t           blank_padding,  /* whether to pad insuffizient left fieldwidth with blanks or zeros */
                            size_t            precision,      /* specified precision or (~(size_t) 0) if unspecified */
                            size_t            fieldwidth)     /* minimum field width */
{
    size_t zRet = 0;
    const char * padding  = (blank_padding || (precision != ~(size_t) 0)) ? pblanks : pzeros; /* padding characters in front of the output value */
    size_t       minwidth = (precision == ~(size_t)0) ? 1 : precision;                        /* minimum width of value to print */
    const char * digit    = (format & 0x20) ? lower_digits : upper_digits;                    /* digits to use */
    uint8_t      base     = PrintfIntBase[(unsigned char) format];                            /* numeric base system for the output data */
    char         buf[80];                                                                     /* buffer for temporary output of value */
    char *       pe       = buf + sizeof(buf);
    char *       ps       = pe;
    uint64_t     x        = value;

    if(base < 2)
       goto Exit;

    if (!x)
    {
       prefixing = 0; /* no prefixing of 0x according to the C standard */

       if (precision)  /* required by C and Posix standards :o( */
          *--ps = '0';
    }
    else if (base == 10)
    {
       const char * pd;

       while (x >= 100)
       {
          uint64_t tmp = x;
          x /= 100;
          pd = d00 + ((tmp - x * 100) * 2);
          ps -= 2;
          ps[0] = pd[0];
          ps[1] = pd[1];
       }

       if(x >= 10)
       {
          pd = d00 + (x * 2);
          ps -= 2;
          ps[0] = pd[0];
          ps[1] = pd[1];
       }
       else
          *--ps = digit[x];
    }
    else if (base == 16)
    {
       do
       {
          *--ps = digit[x & 0xf];
          x >>= 4;
       } while (x);
    }
    else if (base == 8)
    {
       do
       {
          *--ps = digit[x & 0x7];
          x >>= 3;
       } while (x);
    }
    else if (base == 2)
    {
       do
       {
          *--ps = digit[x & 0x1];
          x >>= 1;
       } while (x);
    }
    else
    {
       do
       {
          uint64_t tmp = x;
          x /= base;
          *--ps = digit[tmp - (x * base)];
       }  while (x);
    }

    zRet = cbk_print_number(pUserData, pCB, ps, (size_t)(pe - ps), padding, sign_char, prefixing ? base : 0, digit == upper_digits, left_justified, minwidth, fieldwidth);

    Exit:;
    return (zRet);
} /* size_t cbk_print_u64(...) */



/* ------------------------------------------------------------------------- *\
   cbk_print_u32 prints an uint32_t using a printf callback function
   and returns the written string data length.
\* ------------------------------------------------------------------------- */

static size_t cbk_print_u32(void *            pUserData,      /* user specific context for the callback */
                            PRINTF_CALLBACK * pCB,            /* data write callback */
                            uint32_t          value,          /* value to write to the string */
                            char              format,         /* format character in the format string */
                            char              sign_char,      /* expected sign character */
                            uint8_t           prefixing,      /* C style prefixing of octals and hexadecimals required */
                            uint8_t           left_justified, /* left justified value witin the output data field */
                            uint8_t           blank_padding,  /* whether to pad insuffizient left fieldwidth with blanks or zeros */
                            size_t            precision,      /* specified precision or (~(size_t) 0) if unspecified */
                            size_t            fieldwidth)     /* minimum field width */
{
    size_t zRet = 0;
    const char * padding  = (blank_padding || (precision != ~(size_t) 0)) ? pblanks : pzeros; /* padding characters in front of the output value */
    size_t       minwidth = (precision == ~(size_t)0) ? 1 : precision;                        /* minimum width of value to print */
    const char * digit    = (format & 0x20) ? lower_digits : upper_digits;                    /* digits to use */
    uint8_t      base     = PrintfIntBase[(unsigned char) format];                            /* numeric base system of for the output data */
    char         buf[80];                                                                     /* buffer for temporary output of value */
    char *       pe       = buf + sizeof(buf);
    char *       ps       = pe;
    uint32_t     x        = value;

    if(base < 2)
       goto Exit;

    if (!x)
    {
       prefixing = 0; /* no prefixing of 0x according to the C standard */

       if (precision)  /* required by C and Posix standards :o( */
          *--ps = '0';
    }
    else if (base == 10)
    {
       const char * pd;

       while (x >= 100)
       {
          uint32_t tmp = x;
          x /= 100;
          pd = d00 + ((tmp - x * 100) * 2);
          ps -= 2;
          ps[0] = pd[0];
          ps[1] = pd[1];
       }

       if(x >= 10)
       {
          pd = d00 + (x * 2);
          ps -= 2;
          ps[0] = pd[0];
          ps[1] = pd[1];
       }
       else
          *--ps = digit[x];
    }
    else if (base == 16)
    {
       do
       {
          *--ps = digit[x & 0xf];
          x >>= 4;
       } while (x);
    }
    else if (base == 8)
    {
       do
       {
          *--ps = digit[x & 0x7];
          x >>= 3;
       } while (x);
    }
    else if (base == 2)
    {
       do
       {
          *--ps = digit[x & 0x1];
          x >>= 1;
       } while (x);
    }
    else
    {
       do
       {
          uint32_t tmp = x;
          x /= base;
          *--ps = digit[tmp - (x * base)];
       }  while (x);
    }

    zRet = cbk_print_number(pUserData, pCB, ps, (size_t)(pe - ps), padding, sign_char, prefixing ? base : 0, digit == upper_digits, left_justified, minwidth, fieldwidth);

    Exit:;
    return (zRet);
} /* size_t cbk_print_u32(...) */


/* ------------------------------------------------------------------------- *\
   Floating point related routines
\* ------------------------------------------------------------------------- */


#if !defined(_WIN32) || (LDBL_MANT_DIG != DBL_MANT_DIG)
/* MSVC does not support 80 or 128 bit floating points :o( */

/* ------------------------------------------------------------------------- *\
   base10 calculates mantissa and exponent of a given value in decimal system.
   base10l is just a for base 10 slightly optimized version of rebasel.
   Just read the comments of rebasel for details about the algorithm.
\* ------------------------------------------------------------------------- */

static void base10l(long double value, long double * mantissa, int32_t * exponent)
{
   static struct basepows_s {  long double p; long double rp; int32_t e; } basepow [16] = {{1e+1l, 1e-1l, 1}, {1e+2l, 1e-2l, 2}, {1e+4l, 1e-4l, 4}, {1e+8l, 1e-8l, 8}, {1e+16l, 1e-16l, 16}, {1e+32l, 1e-32l, 32}, {1e+64l, 1e-64l, 64}, {1e+128l, 1e-128l, 128},
                                                                                          {1e+256l, 1e-256l, 256}, {1e+512l, 1e-512l, 512}, {1e+1024l, 1e-1024l, 1024}, {1e+1024l, 1e-1024l, 1024}, {1e+2048l, 1e-2048l, 2048}, {1e+4096l, 1e-4096l, 4096}, {0.0, 0.0, 0}};
   struct basepows_s * pb = basepow;
   int32_t expo = 0;
   int     sign = value < 0;
   long double  mant = sign ? -value : value;

   if (mant >= 10.0l)
   {
      while ((pb->p <= mant) && (++pb)->e)
      {
      }

      while(pb-- != basepow)
      {
         if(pb->p <= mant)
         {
            mant *= pb->rp;
            expo += pb->e;
         }
      }
   }
   else if (mant < 1.0l)
   {
      while (10.0l > (pb->p * mant) && (++pb)->e)
      {
      }

      while(pb-- != basepow)
      {
         if(10.0l > (mant * pb->p))
         {
            mant *= pb->p;
            expo -= pb->e;
         }
      }
   }

   * mantissa = sign ? -mant : mant;
   * exponent = expo;
} /* void base10l(double value, double * mantissa, int32_t * exponent) */
#endif /* _WIN32 */


/* ------------------------------------------------------------------------- *\
   base10 calculates mantissa and exponent of a given value in decimal system.
   base10 is just a for base 10 slightly optimized version of rebase.
   Just read the comments of rebase for details about the algorithm.
\* ------------------------------------------------------------------------- */

static void base10(double value, double * mantissa, int32_t * exponent)
{
   static struct basepows_s {  double p; double rp; int32_t e; } basepow [16] = {{1e+1, 1e-1, 1}, {1e+2, 1e-2, 2}, {1e+4, 1e-4, 4}, {1e+8, 1e-8, 8}, {1e+16, 1e-16, 16}, {1e+32, 1e-32, 32}, {1e+64, 1e-64, 64}, {1e+128, 1e-128, 128}, {1e+256, 1e-256, 256}, {0.0, 0.0, 0}};
   struct basepows_s * pb = basepow;
   int32_t expo = 0;
   int     sign = value < 0;
   double  mant = sign ? -value : value;

   if (mant >= 10.0)
   {
      while ((pb->p <= mant) && (++pb)->e)
      {
      }

      while(pb-- != basepow)
      {
         if(pb->p <= mant)
         {
            mant *= pb->rp;
            expo += pb->e;
         }
      }
   }
   else if (mant < 1.0)
   {
      while (10.0 > (pb->p * mant) && (++pb)->e)
      {
      }

      while(pb-- != basepow)
      {
         if(10.0 > (mant * pb->p))
         {
            mant *= pb->p;
            expo -= pb->e;
         }
      }
   }

   * mantissa = sign ? -mant : mant;
   * exponent = expo;
} /* void base10(double value, double * mantissa, int32_t * exponent) */



/* ------------------------------------------------------------------------- *\
   rebasel calculates mantissa and exponent of a long double value for
   a new base. It is pretty fast but not exact for the last digits of the
   mantissa. How does the magic work?
   We need to divide the value by the highest exponential value of the base
   that is lower than just our value for finding the mantissa.
   But powers of 10 (and other bases as well) can be split as in the
   following example : 10^11 = 10^8 * 10^2 * 10^1
   A multiplication of a power of the base with itself just duplicates
   the exponent. That's why every power of ten can be split in a sequence
   of factors of a power of ten to a power of two and that is what we do
   for the estimation of the highest power of the base by which we need
   to divide our value for the calculation of the decimal mantissa.
   Because multiplying the base with itself causes a duplication of the
   exponent we are duplicating our exponent repeatedly by squaring the
   the base repeatedly until the result becomes larger then our input value.
   The square value of the base may become +INF if it exceeds the highest
   float value and be above every regular floating point number then.
   Afterwards we divide the value by all the powers of our base that are
   smaller then the remaining value and calculate the summary of the related
   exponents for finding the exponent of the value within the new base system.
   How to improve this?
    - precalculate the subsequent square values of the base but if possible
      use a higher precision for the calculation
    - multiply with reciprocals of the square values instead using the division.
      This may lead to a higher inaccuracy if the values are calculated at
      runtime. The algorithm is pretty fast despite of the divisions.
    - we could do a halb binary search for the position of our value in the
      array of the subsequent powers of the squares of the base
    - improve the precision by using the exponent as index of an array of
      all possible powers of the base and multiply or divide the original
      value by that
    - we could also do a half binary search for the position of our initiial
      value in an array of all possible powers of our base and divide or
      multiply the value by that
\* ------------------------------------------------------------------------- */

static void rebasel(long double value, uint32_t base, long double * mantissa, int32_t * exponent)
{
   if(base == 10)
   { /* try to be a little bit more exact by using base10l or base10 which are using a table of precalculated decimal exponent values */
#if !defined(_WIN32) || (LDBL_MANT_DIG != DBL_MANT_DIG)
      base10l(value, mantissa, exponent);
#else
      base10(value, mantissa, exponent);
#endif
   }
   else
   {
      struct basepows_s {long double p; int32_t e; } basepow [32];
      struct basepows_s * pb = basepow;
      int32_t      expo = 0;
      int          sign = value < 0;
      long double  mant = sign ? -value : value;
      long double  p    = base; /* holds powers of the base */

      if (mant >= p)
      {
         expo = 1;

         while (p <= mant)
         {
            pb->p = p;
            pb->e = expo;
            ++pb;

            expo += expo;
            p *= p;
         }

         expo = 0;
         p    = 1.0;

         while(pb != basepow)
         {
            --pb;
            if((p * pb->p) <= mant)
            {
               p *= pb->p;
               expo += pb->e;
            }
         }

         mant /= p;
      }
      else if (mant < 1.0)
      {
         expo = -1;

         while ((long double) base > (p * mant))
         {
            pb->p = p;
            pb->e = expo;
            ++pb;

            expo += expo;
            p *= p;
         }

         expo = 0;
         p = base;

         while(pb != basepow)
         {
            --pb;
            if(p > (mant * pb->p))
            {
               mant *= pb->p;
               expo += pb->e;
            }
         }
      }

      * mantissa = sign ? -mant : mant;
      * exponent = expo;
   }
} /* void rebasel(long double value, uint32_t base, long double * mantissa, int32_t * exponent) */



/* ------------------------------------------------------------------------- *\
   rebase calculates mantissa and exponent of a double value for a new base.
   It is pretty fast but not exact for the last digits of the mantissa.
   How does the magic work?
   We need to divide the value by the highest exponential value of the base
   that is lower than just our value for finding the mantissa.
   But powers of 10 (and other bases as well) can be split as in the
   following example : 10^11 = 10^8 * 10^2 * 10^1
   A multiplication of a power of the base with itself just duplicates
   the exponent. That's why every power of ten can be split in a sequence
   of factors of a power of ten to a power of two and that is what we do
   for the estimation of the highest power of the base by which we need
   to divide our value for the calculation of the decimal mantissa.
   Because multiplying the base with itself causes a duplication of the
   exponent we are duplicating our exponent repeatedly by squaring the
   the base repeatedly until the result becomes larger then our input value.
   The square value of the base may become +INF if it exceeds the highest
   float value and be above every regular floating point number then.
   Afterwards we divide the value by all the powers of our base that are
   smaller then the remaining value and calculate the summary of the related
   exponents for finding the exponent of the value within the new base system.
   How to improve this?
    - precalculate the subsequent square values of the base but if possible
      use a higher precision for the calculation
    - multiply with reciprocals of the square values instead using the division.
      This may lead to a higher inaccuracy if the values are calculated at
      runtime. The algorithm is pretty fast despite of the divisions.
    - we could do a halb binary search for the position of our value in the
      array of the subsequent powers of the squares of the base
    - improve the precision by using the exponent as index of an array of
      all possible powers of the base and multiply or divide the original
      value by that
    - we could also do a half binary search for the position of our initiial
      value in an array of all possible powers of our base and divide or
      multiply the value by that
\* ------------------------------------------------------------------------- */

static void rebase(double value, uint32_t base, double * mantissa, int32_t * exponent)
{
   if(base == 10)
   { /* try to be a little bit more exact by using base10 which is using a table of precalculated decimal exponent values */
      base10(value, mantissa, exponent);
   }
   else
   {
      struct basepows_s {  double p; int32_t e; } basepow [20];
      struct basepows_s * pb = basepow;
      int32_t expo = 0;
      int     sign = value < 0;
      double  mant = sign ? -value : value;
      double  p    = base; /* holds powers of the base */

      if (mant >= p)
      {
         expo = 1;

         while (p <= mant)
         {
            pb->p = p;
            pb->e = expo;
            ++pb;

            expo += expo;
            p *= p;
         }

         expo = 0;
         p    = 1.0;

         while(pb != basepow)
         {
            --pb;
            if((p * pb->p) <= mant)
            {
               p *= pb->p;
               expo += pb->e;
            }
         }

         mant /= p;
      }
      else if (mant < 1.0)
      {
         expo = -1;

         while ((double) base > (p * mant))
         {
            pb->p = p;
            pb->e = expo;
            ++pb;

            expo += expo;
            p *= p;
         }

         expo = 0;
         p = base;

         while(pb != basepow)
         {
            --pb;
            if(p > (mant * pb->p))
            {
               mant *= pb->p;
               expo += pb->e;
            }
         }
      }

      * mantissa = sign ? -mant : mant;
      * exponent = expo;
   }
} /* void rebase(double value, uint32_t base, double * mantissa, int32_t * exponent) */



/* ------------------------------------------------------------------------- *\
   powil calculates a power of the base and returns it as a long double.
   See comments of rebase() for details.
\* ------------------------------------------------------------------------- */

static long double powil (uint8_t base, int32_t iexpo)
{
   long double val  = 1.0;
   long double p    = base;
   uint32_t    expo = iexpo >= 0 ? iexpo : -iexpo;

   if (expo)
   {
      if (expo & 1)
          val = p;

      expo >>= 1;
      while (expo)
      {
         p *= p;

         if (expo & 1)
            val *= p;

         expo >>= 1;
      }
   }
   return (iexpo < 0 ? (1.0 / val) : val);
} /* long double powil (uint8_t base, int32_t iexpo) */


/* ------------------------------------------------------------------------- *\
   powi calculates a power of the base and returns it as a double.
   See comments of rebase() for details.
\* ------------------------------------------------------------------------- */

static double powi (uint8_t base, int32_t iexpo)
{
   double   val  = 1.0;
   double   p    = base;
   uint32_t expo = iexpo >= 0 ? iexpo : -iexpo;

   if (expo)
   {
      if (expo & 1)
          val = p;

      expo >>= 1;
      while (expo)
      {
         p *= p;

         if (expo & 1)
            val *= p;

         expo >>= 1;
      }
   }

   return (iexpo < 0 ? (1.0 / val) : val);
} /* double powi (uint8_t base, int32_t iexpo) */


/* ------------------------------------------------------------------------- *\
   print_long_double_e prints an unsigned long double in '%Le' format to a
   buffer and returns the written string data length.
\* ------------------------------------------------------------------------- */

static size_t print_long_double_e(char *       pBuf,       /* pointer to buffer */
                                  long double  mant,       /* mantissa */
                                  int32_t      iexpo,      /* exponent */
                                  uint8_t      base,       /* base system to use */
                                  size_t       minwidth,   /* minimum width of mantissa to print */
                                  uint8_t      prefixing,  /* whether to add a decimal point even if the mantissa is zero */
                                  const char * digit)      /* array of digits to use */
{
   char * pb = pBuf;
   uint32_t count;

#if 1
   if(minwidth)
   {
      uint64_t m1 = (uint64_t) (mant * 0x400000000000000ll);                              /* upper part of mantissa */
      uint64_t m2 = (uint64_t) ((mant * 0x400000000000000ll - m1) * 0x400000000000000ll); /* lower part of mantissa */
      uint8_t b = base ? base : 16;

      *pb++ = digit[(size_t) (m1 >> 58)];
      *pb++ = '.';

      do
      {
         m1    &= 0x3ffffffffffffffll;;
         m2    *= b;
         m1    *= b;
         m1    += m2 >> 58;
         m2    &= 0x3ffffffffffffffll;
         *pb++ = digit[(size_t) (m1 >> 58)];
      }
      while(--minwidth);
   }
   else
   {
      *pb++ = digit[(size_t) mant];

      if(prefixing)
         *pb++ = '.';
   }
#else
   long double dbase = base ? base : 16;

   count = (uint32_t) mant;
   *pb++ = digit[count];

   if(minwidth)
   {
      *pb++ = '.';

      do
      {
         mant  = (mant - count) * dbase;
         count = (uint32_t) mant;
         *pb++ = digit[count];
      } while (--minwidth);
   }
   else
   {
      if(prefixing)
         *pb++ = '.';
   }
#endif

   /* write the exponent */
   if (!base)
      *pb++ = digit[25]; /* use digit p or P instead of e or E */
   else if (base > 0xe)
      *pb++ = '~';
   else
      *pb++ = digit[0xe];

   if(iexpo < 0)
   {
      *pb++ = '-';
      count = (uint32_t) - iexpo;
   }
   else
   {
      *pb++ = '+';
      count = (uint32_t) iexpo;
   }

   if(count < base)
   { /* the exponent has only 1 digit */
      *pb++ = '0';
      *pb++ = digit[count];
      count = (uint32_t) (pb - pBuf);
   }
   else
   {
      char * ps = pb;

      if(!base)
         base = 10;

      while(count >= base)
      {
         uint32_t tmp = count;
         count /= base;
         *pb++ = digit[tmp - (count * base)];
      }

      *pb++ = digit[count];

      count = (uint32_t) (pb - pBuf);

      while (--pb > ps)
      {  /* invert the sequence of the digits */
         char c = *ps;
         *(ps++) = *pb;
         *pb = c;
      }
   }

   return (count);
} /* size_t print_long_double_e (...) */



/* ------------------------------------------------------------------------- *\
   print_long_double_f prints an unsigned long double in '%Lf' format to a
   character buffer and returns the written string data length.
\* ------------------------------------------------------------------------- */

static size_t print_long_double_f(char *       pBuf,       /* pointer to buffer */
                                  long double  mant,       /* mantissa */
                                  int32_t      iexpo,      /* exponent */
                                  uint8_t      base,       /* base system to use */
                                  size_t       minwidth,   /* minimum width of mantissa to print */
                                  uint8_t      prefixing,  /* whether to add a decimal point even if the mantissa is zero */
                                  const char * digit)      /* array of digits to use */
{
   char * pb = pBuf;

#if 1
   if(iexpo < 0)
   {
      ++iexpo;
      *pb++ = '0';

      if(minwidth)
      {
         uint64_t m1 = (uint64_t) (mant * 0x400000000000000ll);                              /* upper part of mantissa */
         uint64_t m2 = (uint64_t) ((mant * 0x400000000000000ll - m1) * 0x400000000000000ll); /* lower part of mantissa */

         *pb++ = '.';

         while (iexpo < 0)
         {
            *pb++ = '0';
            if(!--minwidth)
               goto Exit;
            ++iexpo;
         }

         *pb++ = digit[(size_t) (m1 >> 58)];

         while(--minwidth)
         {
            m1    &= 0x3ffffffffffffffll;
            m2    *= base;
            m1    *= base;
            m1    += m2 >> 58;
            m2    &= 0x3ffffffffffffffll;
            *pb++ = digit[(size_t) (m1 >> 58)];
         }
      }
      else
      {
         if(prefixing)
            *pb++ = '.';
      }
   }
   else
   {
      uint64_t m1 = (uint64_t) (mant * 0x400000000000000ll);                              /* upper part of mantissa */
      uint64_t m2 = (uint64_t) ((mant * 0x400000000000000ll - m1) * 0x400000000000000ll); /* lower part of mantissa */

      ++iexpo;

      *pb++ = digit[(size_t) (m1 >> 58)];

      while(--iexpo)
      {
         m1    &= 0x3ffffffffffffffll;
         m2    *= base;
         m1    *= base;
         m1    += m2 >> 58;
         m2    &= 0x3ffffffffffffffll;

         *pb++ = digit[(size_t) (m1 >> 58)];
      }

      if(minwidth)
      {
         *pb++ = '.';

         do
         {
            m1    &= 0x3ffffffffffffffll;
            m2    *= base;
            m1    *= base;
            m1    += m2 >> 58;
            m2    &= 0x3ffffffffffffffll;
            *pb++ = digit[(size_t) (m1 >> 58)];
         } while(--minwidth);
      }
      else
      {
        if(prefixing)
           *pb++ = '.';
      }
   }
   Exit:;
#else
   long double dbase = base;

   if(iexpo < 0)
   {
      ++iexpo;
      *pb++ = '0';
   }
   else
   {
      ++iexpo;
      do
      {
         uint32_t count = (uint32_t) mant;
         *pb++ = digit[count];
         mant  = (mant - count) * dbase;
      } while(--iexpo);
   }

   if(minwidth)
   {
      *pb++ = '.';

      while (minwidth && (iexpo < 0))
      {
         *pb++ = '0';
         ++iexpo;
         --minwidth;
      }

      while (minwidth--)
      {
         uint32_t count = (uint32_t) mant;
         mant  = (mant - count) * dbase;
         *pb++ = digit[count];
      }
   }
   else
   {
      if(prefixing)
         *pb++ = '.';
   }
#endif

   return (pb - pBuf);
} /* size_t print_long_double_f (...) */



/* ------------------------------------------------------------------------- *\
   smemcpy a small local memcpy wrapper
\* ------------------------------------------------------------------------- */

static void smemcpy(void * pdst, const void * psrc, size_t count)
{
   uint8_t * pd = (uint8_t *) pdst;
   const uint8_t * ps = (const uint8_t *) psrc;

   while(count--)
      *pd++ = *ps++;
} /* static void smemcpy(void * pdst, void *psrc, size_t count) */



/* ------------------------------------------------------------------------- *\
   sstrcpy a small local strcpy wrapper
\* ------------------------------------------------------------------------- */

static void sstrcpy(char * pdst, const char * psrc)
{
   while((*pdst++ = *psrc++))
   {};
} /* static sstrcpy(char * pdst, const char * psrc) */

/* ------------------------------------------------------------------------- *\
   cbk_print_long_double prints a double using a callback_printf callback
   function and returns the written string data length.
\* ------------------------------------------------------------------------- */

static size_t cbk_print_long_double(void *            pUserData,      /* user specific context for the callback */
                                    PRINTF_CALLBACK * pCB,            /* data write callback */
                                    long double       value,          /* value to write to the string */
                                    uint8_t           base,           /* base system to use */
                                    char              sign_char,      /* expected sign character */
                                    char              format,         /* printf floating point format 'a', 'e', 'f' or 'g' */
                                    uint8_t           prefixing,      /* decimal point even  without digits after it required */
                                    uint8_t           left_justified, /* left justified value witin the output data field */
                                    uint8_t           blank_padding,  /* whether to pad insuffizient left fieldwidth with blanks or zeros */
                                    size_t            minwidth,       /* minimum width of value to print */
                                    size_t            fieldwidth)     /* minimum field width */
{
   size_t zRet = 0;
   const char *  padding   = blank_padding ? pblanks : pzeros;
   uint8_t       uppercase = !(format & 0x20); /* whether to use uppercase letters */
   const char *  digit     = uppercase ? upper_digits : lower_digits;
   char buf[240];
   size_t length = 0;

   /*  test for NAN and INF first */
   if (!(value == value))
   {  /* NAN */
      char * pb = buf;
      float   f = (float) value;
      int32_t i;
      smemcpy(&i, &f, 4);

      if(i < 0)
         *pb++ = '-';
      else if(sign_char)
         *pb++ = sign_char;

      sstrcpy (pb, uppercase ? "NAN" : "nan");
      length = (pb - buf) + 3;
      sign_char = '\0';
      padding = pblanks;
   }
   else if (value < -LDBL_MAX)
   {  /* -INF */
      char * pb = buf;
      sstrcpy (pb, uppercase ? "-INF" : "-inf");
      length = 4;
      sign_char = '\0';
      padding = pblanks;
   }
   else if (value > LDBL_MAX)
   { /* INF */
      char * pb = buf;
      if(sign_char)
         *pb++ = sign_char;
      sstrcpy(pb,  uppercase ? "INF" : "inf");
      length = (pb - buf) + 3;
      sign_char = '\0';
      padding = pblanks;
   }
   else
   {
      long double mant;
      int32_t iexpo;

      if (value == 0.0)
      {
         mant  = 0.0;
         iexpo = 0;
      }
      else
      {
         /* calculate mantisse and exponent for base 10 */
         rebasel(value, base, &mant, &iexpo);

         if(mant < 0.0)
         {
            mant = -mant;
            sign_char = '-';
         }
      }

      if(minwidth > 128)
         minwidth = 128; /* limit precision to 128 digits and prevent a buffer overrun */

      format |= 0x20; /* compare lower case letters only */

      if(format == 'g')
      {
         long double round;
         int32_t E = iexpo;

         if(!minwidth)
            minwidth = 1;

         /* we have to care about rounding according to the required length of the mantissa */
         round = 0.5 / powil(base, (int32_t) (minwidth - 1));

         if(base <= (mant + round))
            ++E; /* If exponent increases because of rounding then we need to agjust the value that we compare width */

         if(((int32_t) minwidth > E) && (E >= -4))
         {
            if (E == iexpo)
               mant += round;
            else
               mant += round * base;

            if(mant >= base)
            {/* ensure that */
               mant /= base;
               ++iexpo;
            }

            length = print_long_double_f(buf, mant, iexpo, base, minwidth - 1 - E, prefixing, digit);  /* print floating point numbers without an exponent and adjusted precision */

            if(!prefixing && (minwidth > (size_t)(E + 1)))
            { /* remove trailing zeros */
               while(length && (buf[length-1] == '0'))
                  --length;

               if(length && (buf[length-1] == '.'))
                  --length;
            }
         }
         else
         {
            /* ensure right rounding according to the required length of the mantissa */
            mant += round;

            if(mant >= base)
            {/* ensure that */
               mant /= base;
               ++iexpo;
            }

            length = print_long_double_e(buf, mant, iexpo, base, minwidth - 1, prefixing, digit); /* print floating point number with adjusted precision */

            if(!prefixing)
            {
               char * ps = buf + 1; /* points to decimal point now */
               char * pe = ps + 1;

               while((size_t) (pe - buf) < length)
               {
                  if(*pe == digit[0xe])
                     break; /* stop at begin of the exponent */

                  if(*pe++ != '0')
                     ps = pe;
               }

               if (ps != pe)
               { /* copy exponent to the position that ps points to and remove the unused zeros */
                  while((size_t) (pe - buf) < length)
                     *ps++ = *pe++;

                  length = ps - buf;
               }
            }
         }

         if(prefixing)
            prefixing = (((base == 16) || (base == 2)) ? base : (uint8_t) 0);
      }
      else if(format == 'a')
      {
         /* ensure right rounding according to the required length of the mantissa */
         mant += 0.5 / powil(16, (int32_t) minwidth);

         if(mant >= base)
         {/* ensure that */
            mant /= base;
            ++iexpo;
         }

         length = print_long_double_e(buf, mant, iexpo, 0, minwidth, prefixing, digit);  /* print floating point number with an exponent */
         prefixing = 16;
      }
      else if((format == 'e') || (iexpo > 80))
      {
         if((format == 'f') && (minwidth < 34))
            minwidth = 34; /* ensure full size of the mantissa */

         /* ensure right rounding according to the required length of the mantissa */
         mant += 0.5 / powil(base, (int32_t) minwidth);

         if(mant >= base)
         {/* ensure that */
            mant /= base;
            ++iexpo;
         }

         length = print_long_double_e(buf, mant, iexpo, base, minwidth, prefixing, digit);  /* print floating point number with an exponent */
         if(prefixing)
            prefixing = (((base == 16) || (base == 2)) ? base : (uint8_t) 0);
      }
      else if(format == 'f')
      {
         /* ensure right rounding according to the required length of the mantissa */
         int32_t p = -(iexpo + (int32_t) minwidth);

         if(p >= 0)
            mant += 0.5 * powi(base, p);
         else
            mant += 0.5 / powi(base, -p);


         if(mant >= base)
         {/* ensure that */
            mant /= base;
            ++iexpo;
         }

         length = print_long_double_f(buf, mant, iexpo, base, minwidth, prefixing, digit);  /* print floating point numbers without an exponent */
         if(prefixing)
            prefixing = (((base == 16) || (base == 2)) ? base : (uint8_t) 0);
      }
   }

   zRet = cbk_print_number(pUserData, pCB, buf, length, padding, sign_char, prefixing, uppercase, left_justified, 0 /* minwidth */, fieldwidth);

   return (zRet);
} /* size_t cbk_print_long_double (...) */



/* ------------------------------------------------------------------------- *\
   print_double_e prints an unsigned double in '%e' format to a buffer
   and returns the written string data length.
\* ------------------------------------------------------------------------- */

static size_t print_double_e(char *       pBuf,       /* pointer to buffer */
                             double       mant,       /* mantissa */
                             int32_t      iexpo,      /* exponent */
                             uint8_t      base,       /* base system to use */
                             size_t       minwidth,   /* minimum width of mantissa to print */
                             uint8_t      prefixing,  /* whether to add a decimal point even if the mantissa is zero */
                             const char * digit)      /* array of digits to use */
{
   char * pb    = pBuf;
   uint32_t count;

#if 1
   if(minwidth)
   {
      uint8_t  b = base ? base : 16;
      uint64_t m = (uint64_t) (mant * 0x400000000000000ll);

      *pb++ = digit[(size_t) (m >> 58)];
      *pb++ = '.';

      do
      {
         m &= 0x3ffffffffffffffll;
         m *= b;
         *pb++ = digit[(size_t) (m >> 58)];
      }
      while(--minwidth);
   }
   else
   {
      *pb++ = digit[(size_t) mant];

      if(prefixing)
         *pb++ = '.';
   }
#else
   double dbase = base ? base : 16;

   count = (uint32_t) mant;
   *pb++ = digit[count];

   if(minwidth)
   {
      *pb++ = '.';

      do
      {
         mant  = (mant - count) * dbase;
         count = (uint32_t) mant;
         *pb++ = digit[count];
      } while (--minwidth);
   }
   else
   {
      if(prefixing)
         *pb++ = '.';
   }
#endif

   /* write the exponent */
   if (!base)
      *pb++ = digit[25]; /* use digit p or P instead of e or E */
   else if (base > 0xe)
      *pb++ = '~';
   else
      *pb++ = digit[0xe];

   if(iexpo < 0)
   {
      *pb++ = '-';
      count = (uint32_t) - iexpo;
   }
   else
   {
      *pb++ = '+';
      count = (uint32_t) iexpo;
   }

   if(count < base)
   { /* the exponent has only 1 digit */
      *pb++ = '0';
      *pb++ = digit[count];
      count = (uint32_t) (pb - pBuf);
   }
   else
   {
      char * ps = pb;

      if(!base)
         base = 10;

      while(count >= base)
      {
         uint32_t tmp = count;
         count /= base;
         *pb++ = digit[tmp - (count * base)];
      }

      *pb++ = digit[count];

      count = (uint32_t) (pb - pBuf);

      while (--pb > ps)
      {  /* invert the sequence of the digits */
         char c = *ps;
         *(ps++) = *pb;
         *pb = c;
      }
   }

   return (count);
} /* size_t print_double_e (...) */



/* ------------------------------------------------------------------------- *\
   print_double_f prints an unsigned double in '%f' format to a buffer
   and returns the written string data length.
\* ------------------------------------------------------------------------- */

static size_t print_double_f(char *       pBuf,       /* pointer to buffer */
                             double       mant,       /* mantissa */
                             int32_t      iexpo,      /* exponent */
                             uint8_t      base,       /* base system to use */
                             size_t       minwidth,   /* minimum width of mantissa to print */
                             uint8_t      prefixing,  /* whether to add a decimal point even if the mantissa is zero */
                             const char * digit)      /* array of digits to use */
{
   char * pb = pBuf;

#if 1
   if(iexpo < 0)
   {
      ++iexpo;
      *pb++ = '0';

      if(minwidth)
      {
         uint64_t m1 = (uint64_t) (mant * 0x400000000000000ll);  /* mantissa */
         *pb++ = '.';

         while (iexpo < 0)
         {
            *pb++ = '0';
            if(!--minwidth)
               goto Exit;
            ++iexpo;
         }

         *pb++ = digit[(size_t) (m1 >> 58)];

         while(--minwidth)
         {
            m1    &= 0x3ffffffffffffffll;
            m1    *= base;
            *pb++ = digit[(size_t) (m1 >> 58)];
         }
      }
      else
      {
         if(prefixing)
            *pb++ = '.';
      }
   }
   else
   {
      uint64_t m1 = (uint64_t) (mant * 0x400000000000000ll);    /* mantissa */

      ++iexpo;

      *pb++ = digit[(size_t) (m1 >> 58)];

      while(--iexpo)
      {
         m1    &= 0x3ffffffffffffffll;
         m1    *= base;
         *pb++ = digit[(size_t) (m1 >> 58)];
      }

      if(minwidth)
      {
         *pb++ = '.';

         do
         {
            m1    &= 0x3ffffffffffffffll;
            m1    *= base;
            *pb++ = digit[(size_t) (m1 >> 58)];
         } while(--minwidth);
      }
      else
      {
        if(prefixing)
           *pb++ = '.';
      }
   }
   Exit:;
#else
   double dbase = base;

   if(iexpo < 0)
   {
      ++iexpo;
      *pb++ = '0';
   }
   else
   {
      ++iexpo;
      do
      {
         uint32_t count = (uint32_t) mant;
         *pb++ = digit[count];
         mant  = (mant - count) * dbase;
      } while(--iexpo);
   }

   if(minwidth)
   {
      *pb++ = '.';

      while (minwidth && (iexpo < 0))
      {
         *pb++ = '0';
         ++iexpo;
         --minwidth;
      }

      while (minwidth--)
      {
         uint32_t count = (uint32_t) mant;
         mant  = (mant - count) * dbase;
         *pb++ = digit[count];
      }
   }
   else
   {
      if(prefixing)
         *pb++ = '.';
   }
#endif

   return (pb - pBuf);
} /* size_t print_double_f (...) */



/* ------------------------------------------------------------------------- *\
   cbk_print_double prints a double using a callback_printf callback
   function and returns the written string data length.
\* ------------------------------------------------------------------------- */

static size_t cbk_print_double(void *            pUserData,      /* user specific context for the callback */
                               PRINTF_CALLBACK * pCB,            /* data write callback */
                               double            value,          /* value to write to the string */
                               uint8_t           base,           /* base system to use */
                               char              sign_char,      /* expected sign character */
                               char              format,         /* printf floating point format 'a', 'e', 'f' or 'g' */
                               uint8_t           prefixing,      /* decimal point even  without digits after it required */
                               uint8_t           left_justified, /* left justified value witin the output data field */
                               uint8_t           blank_padding,  /* whether to pad insuffizient left fieldwidth with blanks or zeros */
                               size_t            minwidth,       /* minimum width of value to print */
                               size_t            fieldwidth)     /* minimum field width */
{
   size_t zRet = 0;
   const char *  padding   = blank_padding ? pblanks : pzeros;
   uint8_t       uppercase = !(format & 0x20); /* whether to use uppercase letters */
   const char *  digit     = uppercase ? upper_digits : lower_digits;
   char buf[128];
   size_t length = 0;

   /*  test for NAN and INF first */
   if (!(value == value))
   {  /* NAN */
      char * pb = buf;
      float   f = (float) value;
      int32_t i;
      smemcpy(&i, &f, 4);

      if(i < 0)
         *pb++ = '-';
      else if(sign_char)
         *pb++ = sign_char;

      sstrcpy (pb, uppercase ? "NAN" : "nan");
      length = (pb - buf) + 3;
      sign_char = '\0';
      padding = pblanks;
   }
   else if (value < -DBL_MAX)
   {  /* -INF */
      char * pb = buf;
      sstrcpy (pb, uppercase ? "-INF" : "-inf");
      length = 4;
      sign_char = '\0';
      padding = pblanks;
   }
   else if (value > DBL_MAX)
   { /* INF */
      char * pb = buf;
      if(sign_char)
         *pb++ = sign_char;
      sstrcpy(pb,  uppercase ? "INF" : "inf");
      length = (pb - buf) + 3;
      sign_char = '\0';
      padding = pblanks;
   }
   else
   {
      double  mant;
      int32_t iexpo;

      if (value == 0.0)
      {
         mant  = 0.0;
         iexpo = 0;
      }
      else
      {
         /* calculate mantisse and exponent for base 10 */
         rebase(value, base, &mant, &iexpo);

         if(mant < 0.0)
         {
            mant = -mant;
            sign_char = '-';
         }
      }

      if(minwidth > 64)
         minwidth = 64; /* limit precision to 64 digits and prevent a buffer overrun */

      format |= 0x20; /* compare lower case letters only */

      if(format == 'g')
      {
         double round;
         int32_t E = iexpo;

         if(!minwidth)
            minwidth = 1;

         /* we have to ensure right rounding according to the required length of the mantissa */
         round = 0.5 / powi(base, (int32_t) (minwidth - 1));
         if(base <= (mant + round))
            ++E; /* If exponent increases because of rounding then we need to agjust the value that we compare width */

         if(((int32_t) minwidth > E) && (E >= -4))
         {
            if (E == iexpo)
               mant += round;
            else
               mant += round * base;

            if(mant >= base)
            {/* ensure that */
               mant /= base;
               ++iexpo;
            }

            length = print_double_f(buf, mant, iexpo, base, minwidth - 1 - E, prefixing, digit);  /* print floating point numbers without an exponent and adjusted precision */

            if(!prefixing && (minwidth > (size_t)(E + 1)))
            { /* remove trailing zeros */
               while(length && (buf[length-1] == '0'))
                  --length;

               if(length && (buf[length-1] == '.'))
                  --length;
            }
         }
         else
         {
            /* ensure right rounding according to the required length of the mantissa */
            mant += round;

            if(mant >= base)
            {/* ensure that */
               mant /= base;
               ++iexpo;
            }

            length = print_double_e(buf, mant, iexpo, base, minwidth - 1, prefixing, digit); /* print floating point number with adjusted precision */

            if(!prefixing)
            {
               char * ps = buf + 1; /* points to decimal point now */
               char * pe = ps + 1;

               while((size_t) (pe - buf) < length)
               {
                  if(*pe == digit[0xe])
                     break; /* stop at begin of the exponent */

                  if(*pe++ != '0')
                     ps = pe;
               }

               if (ps != pe)
               { /* copy exponent to the position that ps points to and remove the unused zeros */
                  while((size_t) (pe - buf) < length)
                     *ps++ = *pe++;

                  length = ps - buf;
               }
            }
         }

         if(prefixing)
            prefixing = (((base == 16) || (base == 2)) ? base : (uint8_t) 0);
      }
      else if(format == 'a')
      {
         /* ensure right rounding according to the required length of the mantissa */
         mant += 0.5 / powi(16, (int32_t) minwidth);

         if(mant >= base)
         {/* ensure that */
            mant /= base;
            ++iexpo;
         }

         length = print_double_e(buf, mant, iexpo, 0, minwidth, prefixing, digit);  /* print floating point number with an exponent */
         prefixing = 16;
      }
      else if((format == 'e') || (iexpo > 48))
      {
         if((format == 'f') && (minwidth < 20))
            minwidth = 20; /* ensure full size of the mantissa */

         /* ensure right rounding according to the required length of the mantissa */
         mant += 0.5 / powi(base, (int32_t) minwidth);

         if(mant >= base)
         {/* ensure that */
            mant /= base;
            ++iexpo;
         }

         length = print_double_e(buf, mant, iexpo, base, minwidth, prefixing, digit);  /* print floating point number with an exponent */
         if(prefixing)
            prefixing = (((base == 16) || (base == 2)) ? base : (uint8_t) 0);
      }
      else if(format == 'f')
      {
         /* ensure right rounding according to the required length of the mantissa */
         int32_t p = -(iexpo + (int32_t) minwidth);

         if(p >= 0)
            mant += 0.5 * powi(base, p);
         else
            mant += 0.5 / powi(base, -p);

         if(mant >= base)
         {/* ensure that */
            mant /= base;
            ++iexpo;
         }

         length = print_double_f(buf, mant, iexpo, base, minwidth, prefixing, digit);  /* print floating point numbers without an exponent */
         if(prefixing)
            prefixing = (((base == 16) || (base == 2)) ? base : (uint8_t) 0);
      }
   }
   zRet = cbk_print_number(pUserData, pCB, buf, length, padding, sign_char, prefixing, uppercase, left_justified, 0 /* minwidth */, fieldwidth);

   return (zRet);
} /* size_t cbk_print_double (...) */



/* ------------------------------------------------------------------------- *\
   cbk_print_char prints a character using a printf callback function and
   returns the written string data length.
\* ------------------------------------------------------------------------- */

static size_t cbk_print_char(void *            pUserData,      /* user specific context for the callback */
                             PRINTF_CALLBACK * pCB,            /* data write callback */
                             char              c,
                             size_t            length,
                             size_t            minimum_width,
                             uint8_t           left_justified)
{
   size_t zRet;

   if (minimum_width <= length)
   {
      zRet = length;
      while (length--)
         pCB(pUserData, &c, 1);
   }
   else
   {
      zRet = minimum_width;

      minimum_width -= length;
      if(left_justified)
         while (length--)
            pCB(pUserData, &c, 1);

      while (minimum_width > 32)
      {
         pCB(pUserData, pblanks, 32);
         minimum_width -= 32;
      }

      if(minimum_width)
         pCB(pUserData, pblanks, minimum_width);

      if(!left_justified)
         while (length--)
             pCB(pUserData, &c, 1);
   }

   return (zRet);
} /* size_t cbk_print_char(...) */



/* ------------------------------------------------------------------------- *\
   cbk_print_string prints a string using a printf callback function and
   returns the written string data length.
\* ------------------------------------------------------------------------- */

size_t cbk_print_string(void *            pUserData,      /* user specific context for the callback */
                        PRINTF_CALLBACK * pCB,            /* data write callback */
                        const char *      ps,
                        size_t            length,
                        size_t            minimum_width,
                        uint8_t           left_justified)
{
   size_t zRet;

   if (minimum_width <= length)
   {
      zRet = length;
      if(length)
         pCB(pUserData, ps, length);
   }
   else
   {
      zRet = minimum_width;

      if(length)
      {
         minimum_width -= length;
         if(left_justified)
            pCB(pUserData, ps, length);
      }

      while (minimum_width > 32)
      {
         pCB(pUserData, pblanks, 32);
         minimum_width -= 32;
      }

      if(minimum_width)
         pCB(pUserData, pblanks, minimum_width);

      if(!left_justified && length)
         pCB(pUserData, ps, length);
   }

   return (zRet);
} /* size_t cbk_print_string(...) */



/* ------------------------------------------------------------------------- *\
   cbk_print_wstring prints a wide character string using a printf callback
   function and returns the written string data length.
\* ------------------------------------------------------------------------- */
static size_t cbk_print_wstring(void *            pUserData,      /* user specific context for the callback */
                                PRINTF_CALLBACK * pCB,            /* data write callback */
                                void *            psrc,           /* pointer to source data */
                                size_t            src_length,     /* length of source data in source characters */
                                size_t            char_size,      /* size of a single character */
                                size_t            minimum_width,  /* minimum output field width */
                                uint8_t           left_justified) /* whether the output should be left justified */
{
   size_t zRet;
   char   buf[2048];
   char * pb;
   size_t length = 0;

   src_length *= char_size;

   iUtf8Encode (NULL, &length, &psrc, &src_length, char_size);  /* calculate the length of output that is related to the source data */

   if (minimum_width <= length)
   {
      zRet = length;

      while(src_length)
      {
         pb = buf;
         length = sizeof(buf);
         iUtf8Encode ((void **)&pb, &length, &psrc, &src_length, char_size);
         pCB(pUserData, buf, pb - buf);
      }
   }
   else
   {
      zRet = minimum_width;

      minimum_width -= length;
      if(left_justified && length)
      {
         while(src_length)
         {
            pb = buf;
            length = sizeof(buf);
            iUtf8Encode ((void **)&pb, &length, &psrc, &src_length, char_size);
            pCB(pUserData, buf, pb - buf);
         }
      }

      while (minimum_width > 32)
      {
         pCB(pUserData, pblanks, 32);
         minimum_width -= 32;
      }

      if(minimum_width)
         pCB(pUserData, pblanks, minimum_width);

      if(!left_justified && length)
      {
         while(src_length)
         {
            pb = buf;
            length = sizeof(buf);
            iUtf8Encode ((void **)&pb, &length, &psrc, &src_length, char_size);
            pCB(pUserData, buf, pb - buf);
         }
      }
   }

   return (zRet);
} /* size_t cbk_print_wstring(...) */



/* ------------------------------------------------------------------------- *\
   callback_printf generates vsnprintf like character output by calling a
   user defined write callback for the parts of the generated character data.
   The function returns the length of the overall written data but does not
   terminate the written output data string.
   See implementation of svsnprintf implementation for a sample of usage.
\* ------------------------------------------------------------------------- */


size_t callback_printf(void * pUserData, PRINTF_CALLBACK * pCB, const char * pFmt, va_list val)
{
   size_t       zRet = 0;
   const char * pf   = pFmt;

   if(!pCB)
      goto Exit;

   if(!pf)
   {
      pCB(pUserData, pf, 0);
      goto Exit;
   }

   while(*pf && (*pf != '%'))
      ++pf;

   if(pf != pFmt)
   { /* print the leading string that contains no argument format specifications */
      zRet = (size_t) (pf - pFmt);
      pCB(pUserData, pFmt, zRet);
      pFmt = pf;  /* end of format string or first format specification */
   }

   while(*pf)
   {
      if(*(++pf) == '%')
      {  /* find begin of next format string and write out all the characters at once */
         const char * pe = pf + 1;
         while(*pe && (*pe != '%'))
            ++pe;

         pCB(pUserData, pf, pe - pf); /* write the string data */
         zRet += (size_t)(pe - pf);
         pf = pe; /* begin of next format to check */
      }
      else
      {
         uint8_t blank_padding  = 1;           /* whether to use blanks instead of zeros for legth padding */
         char    sign_char      = '\0';        /* '+' or ' ' for prefixing positive integers with plus */
         uint8_t prefixing      = 0;           /* '#' flag for prefix octals with '0', hexadecimals and pointers with '0x' or '0X' or adding a decimal point to floats */
         uint8_t left_justified = 0;           /* '-' flag left justified output */
         size_t  precision      = ~(size_t) 0; /* precision of output width */
         size_t  minimum_width  = 0;           /* minimum output width */

         const char * ps = pf;
         const char * pe = ps;
         char fc; /* format character */

         if(!IS_PRINTF_FMT_END(*pe))
         {
            if (IS_DIGIT(*ps))
            {
               /* size_t idx = (*ps - '0'); */

               while(IS_DIGIT(*(++ps)))
               {
                  /* idx = (idx * 10) + (*ps - '0'); */
               }

               if (*ps == '$')
               { /* we do not yet handle indexed arguments */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }

               ps = pf; /* may be a minimum width */
            }

            while(IS_PRINTF_FMT_FLAG(*ps))
            {
               if(*ps == '0')
                  blank_padding = 0;
               else if(*ps == '-')
                  left_justified = 1;
               else if(*ps == '#')
                  prefixing = 1;
               else if(*ps == '+')
                  sign_char = '+';
               else if(*ps == ' ')
               {
                  if(sign_char != '+')
                     sign_char = ' ';
               }
               ++ps;
            }

            if(IS_DIGIT(*ps))
            {
               minimum_width = *ps++ - '0';
               while(IS_DIGIT(*ps))
                  minimum_width = (minimum_width * 10) + (*ps++ - '0');
            }
            else if(*ps == '*')
            {
               if(IS_DIGIT(*(++ps)))
               { /* handle indexed arguments */
                  size_t idx = (*ps - '0');

                  while(IS_DIGIT(*(++ps)))
                     idx = (idx * 10) + (*ps - '0');

                  if (*ps != '$')
                  { /* invalid format */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }

                  /* we do not yet support this :o( */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
               else
               {
                  int i = va_arg(val, int);

                  if(i < 0)
                  {/* handle this according to the C standard */
                     left_justified = 1;
                     i = -i;
                  }

                  minimum_width = (size_t) i;
               }
            }

            if(left_justified)
               blank_padding = 1;

            if(*ps == '.')
            {
               if(IS_DIGIT(*(++ps)))
               {
                  precision = *ps++ - '0';
                  while(IS_DIGIT(*ps))
                     precision = (precision * 10) + (*ps++ - '0');
               }
               else if(*ps == '*')
               {
                  if(IS_DIGIT(*(++ps)))
                  { /* handle indexed arguments */
                     size_t idx = (*ps - '0');

                     while(IS_DIGIT(*(++ps)))
                        idx = (idx * 10) + (*ps - '0');

                     if (*ps != '$')
                     { /* invalid format */
                        pCB(pUserData, ps, 0);
                        goto Exit;
                     }

                     /* we do not yet support this :o( */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }
                  else
                  {
                     int i = va_arg(val, int);
                     if(i >= 0)
                        precision = (size_t) i;
                  }
               }
            }

            /*  Let's find the end of the format specification. */
            pe = ps;
            while(!IS_PRINTF_FMT_END(*pe))
               ++pe;
         }

         /* pe points to the terminating format character now while ps points to the begin of type specification. */
         fc = *pe;

         if(fc == 's')
         {
            if(pe == ps)
            {
               const char * pa = va_arg(val, char *);
               size_t length = precision; /* maximum length to be printed */
               if(!pa)
                  pa = "<NULL>";
               ps = pa;
               while (length-- && *ps)
                  ++ps;

               length = (size_t) (ps - pa); /* contains string len to be printed now */
               zRet += cbk_print_string(pUserData, pCB, pa, length, minimum_width, left_justified);
            }
            else if(pe == (ps + 1))
            {
               char s0 = *ps;
               if(s0 == 'l')
               {
                  wchar_t * pa = va_arg(val, wchar_t *);
                  wchar_t * pe = pa;
                  size_t length = precision; /* maximum length to be printed */
                  if(!pa)
                  {
                     pa = (wchar_t *) L"<NULL>";
                     pe = pa;
                  }

                  while (length-- && *pe)
                     ++pe;

                  length = (size_t) (pe - pa); /* contains string len to be printed now */
                  zRet += cbk_print_wstring(pUserData, pCB, pa, length, sizeof(wchar_t), minimum_width, left_justified);
               }
               else
               {
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else if(pe == (ps + 2))
            {
               char s0 = *ps;
               if(s0 == 'l')
               {
                  char s1 = *(ps+1);
                  if(s1 == '4')
                  { /* character of 4 bytes width */
                     static uint32_t warn[] = {'<', 'N', 'U', 'L', 'L', '>', '\0'};
                     uint32_t * pa = va_arg(val, uint32_t *);
                     uint32_t * pe = pa;
                     size_t length = precision; /* maximum length to be printed */

                     if(!pa)
                     {
                        pa = warn;
                        pe = pa;
                     }

                     while (length-- && *pe)
                        ++pe;

                     length = (size_t) (pe - pa); /* contains string len to be printed now */
                     zRet += cbk_print_wstring(pUserData, pCB, pa, length, sizeof(uint32_t), minimum_width, left_justified);
                  }
                  else if(s1 == '2')
                  { /* character of 2 bytes width */
                     static uint16_t warn[] = {'<', 'N', 'U', 'L', 'L', '>', '\0'};
                     uint16_t * pa = va_arg(val, uint16_t *);
                     uint16_t * pe = pa;
                     size_t length = precision; /* maximum length to be printed */

                     if(!pa)
                     {
                        pa = warn;
                        pe = pa;
                     }

                     while (length-- && *pe)
                        ++pe;

                     length = (size_t) (pe - pa); /* contains string len to be printed now */
                     zRet += cbk_print_wstring(pUserData, pCB, pa, length, sizeof(uint16_t), minimum_width, left_justified);
                  }
                  else if(s1 == '1')
                  { /* character of 1 byte width */
                     static uint8_t warn[] = {'<', 'N', 'U', 'L', 'L', '>', '\0'};
                     uint8_t * pa = va_arg(val, uint8_t *);
                     uint8_t * pe = pa;
                     size_t length = precision; /* maximum length to be printed */

                     if(!pa)
                     {
                        pa = warn;
                        pe = pa;
                     }

                     while (length-- && *pe)
                        ++pe;

                     length = (size_t) (pe - pa); /* contains string len to be printed now */
                     zRet += cbk_print_wstring(pUserData, pCB, pa, length, sizeof(uint8_t), minimum_width, left_justified);
                  }
                  else
                  { /* unknown format */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else
            { /* unknown format */
               pCB(pUserData, ps, 0);
               goto Exit;
            }
         }
         else if(fc == 'c')
         {
            if(pe == ps)
            {
               char c = (char) va_arg(val, int);
               zRet += cbk_print_char(pUserData, pCB, c, (precision == ~(size_t) 0) ? 1 : precision, minimum_width, left_justified);
            }
            else if(pe == (ps + 1))
            {
               char s0 = *ps;
               if(s0 == 'l')
               {
                  wchar_t wc = (wchar_t) va_arg(val, unsigned int);
                  zRet += cbk_print_wstring(pUserData, pCB, &wc, 1, sizeof(wchar_t), minimum_width, left_justified);
               }
               else
               {
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else if(pe == (ps + 2))
            {
               char s0 = *ps;
               if(s0 == 'l')
               {
                  char s1 = *(ps+1);

                  if(s1 == '4')
                  { /* character of 4 bytes width */
                     uint32_t wc = va_arg(val, uint32_t);
                     zRet += cbk_print_wstring(pUserData, pCB, &wc, 1, sizeof(uint32_t), minimum_width, left_justified);
                  }
                  else if(s1 == '2')
                  { /* character of 2 bytes width */
                     uint16_t wc = (uint16_t) va_arg(val, unsigned int);
                     zRet += cbk_print_wstring(pUserData, pCB, &wc, 1, sizeof(uint16_t), minimum_width, left_justified);
                  }
                  else if(s1 == '1')
                  { /* character of 1 byte width */
                     uint8_t wc = (uint8_t) va_arg(val, unsigned int);
                     zRet += cbk_print_wstring(pUserData, pCB, &wc, 1, sizeof(uint8_t), minimum_width, left_justified);
                  }
                  else
                  { /* unknown format */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }
               }
               else
               {
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else
            { /* unknown format */
               pCB(pUserData, ps, 0);
               goto Exit;
            }
         }
         else if (IS_PRINTF_FMT_INT(fc))
         {
/* ------------------------------------------------------------------------- */
#define CHECK_SIGN(itype, utype, va_itype, va_utype) \
                   utype u;\
                   if((fc == 'd') || (fc == 'i'))\
                   {\
                      itype i = (itype) va_arg(val, va_itype);\
                      if(i >= 0)\
                      {\
                         u = (utype) i;\
                      }\
                      else\
                      {/* negative signed integer -> convert it to a positive unsigned one and set the sign character */\
                         u = (utype) ~i + 1;\
                         sign_char = '-';\
                      }\
                   }\
                   else\
                   {\
                      u = (utype) va_arg(val, va_utype);\
                      sign_char = '\0'; /* do not print any sign character */\
                   }
/* ------------------------------------------------------------------------- */
            if(pe == ps)
            {
               CHECK_SIGN (int, unsigned int, int, unsigned int);

               if(sizeof(u) <= 4)
                   zRet += cbk_print_u32(pUserData, pCB, u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
               else
                   zRet += cbk_print_u64(pUserData, pCB, u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
            }
            else if(pe == (ps + 1))
            {
               char s0 = *ps;
               if(s0 == 'l')
               {
                  CHECK_SIGN (long, unsigned long, long, unsigned long);

                  if(sizeof(u) <= 4)
                     zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  else
                     zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
               }
               else if((s0 == 'z') || (s0 == 't') || (s0 == 'I'))
               {
                  CHECK_SIGN (ptrdiff_t, size_t, ptrdiff_t, size_t);

                  if(sizeof(u) <= 4)
                     zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  else
                     zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
               }
               else if(s0 == 'h')
               {
                  CHECK_SIGN (short, unsigned short, int, unsigned int);
                  zRet += cbk_print_u32(pUserData, pCB, u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
               }
               else if(s0 == 'j')
               {
                  CHECK_SIGN (intmax_t, uintmax_t, intmax_t, uintmax_t);
                  zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else if(pe == (ps + 2))
            {
               char s0 = *ps;
               char s1 = *(ps+1);

               if(s0 == 'l')
               {
                  if(s1 == '8')
                  { /* integer of 8 bytes width */
                     CHECK_SIGN (int64_t, uint64_t, int64_t, uint64_t);
                     zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else if(s1 == '4')
                  { /* integer of 4 bytes width */
                     CHECK_SIGN (int32_t, uint32_t, int32_t, uint32_t);
                     zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else if(s1 == '2')
                  { /* integer of 2 bytes width */
                     CHECK_SIGN (int16_t, uint16_t, int, unsigned int);
                     zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else if(s1 == '1')
                  { /* integer of 1 byte width */
                     CHECK_SIGN (int8_t, uint8_t, int, unsigned int);
                     zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else if(s1 == 'l')
                  { /* argument of type long long */
                     CHECK_SIGN (long long, unsigned long long, long long, unsigned long long);
                     zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else
                  { /* unknown format */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }
               }
               else if((s0 == 'h') && (s1 == 'h'))
               {
                  CHECK_SIGN (signed char, unsigned char, int, unsigned int);
                  zRet += cbk_print_u32(pUserData, pCB, u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
               }
               else if (s0 == 'r')
               {
                  uint8_t base;
                  if(s1 == '*')
                  {
                     base = (uint8_t) va_arg(val, int);

                     if(base > 36)
                     { /* unsupported base */
                        pCB(pUserData, ps, 0);
                        goto Exit;
                     }
                  }
                  else if(IS_DIGIT(s1))
                  {
                     base = (uint8_t) (s1 - '0');
                  }
                  else
                  { /* unsupported base */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }

                  {
                     CHECK_SIGN (int, unsigned int, int, unsigned int);
                     zRet += cbk_print_u32(pUserData, pCB, u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else if(pe == (ps + 3))
            {
               char s0 = *ps;
               char s1 = *(ps+1);
               char s2 = *(ps+2);

               if (s0 =='I')
               {
                  if((s1 == '6') && (s2 == '4'))
                  { /* integer of 8 bytes width */
                     CHECK_SIGN (int64_t, uint64_t, int64_t, uint64_t);
                     zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else if((s1 == '3') && (s2 == '2'))
                  { /* integer of 4 bytes width */
                     CHECK_SIGN (int32_t, uint32_t, int32_t, uint32_t);
                     zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else if((s1 == '1') && (s2 == '6'))
                  { /* integer of 2 bytes width */
                     CHECK_SIGN (int16_t, uint16_t, int, unsigned int);
                     zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, fc, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else
                  { /* unknown format */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }
               }
               else if (s0 == 'r')
               {
                  uint8_t base;

                  if(s1 == '*')
                  {
                     base = (uint8_t) va_arg(val, int);

                     if(base > 36)
                     { /* unsupported base */
                        pCB(pUserData, ps, 0);
                        goto Exit;
                     }
                  }
                  else if (IS_DIGIT(s1))
                  {
                     base = (uint8_t) (s1 - '0');
                  }
                  else
                  { /* unsupported base */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }

                  if(s2 == 'l')
                  {
                     CHECK_SIGN (long, unsigned long, long, unsigned long);

                     if(sizeof(u) <= 4)
                        zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                     else
                        zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else if((s2 == 'z') || (s2 == 't') || (s2 == 'I'))
                  {
                     CHECK_SIGN (ptrdiff_t, size_t, ptrdiff_t, size_t);

                     if(sizeof(u) <= 4)
                        zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                     else
                        zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else if(s2 == 'h')
                  {
                     CHECK_SIGN (short, unsigned short, int, unsigned int);
                     zRet += cbk_print_u32(pUserData, pCB, u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else if(s2 == 'j')
                  {
                     CHECK_SIGN (intmax_t, uintmax_t, intmax_t, uintmax_t);
                     zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else
                  { /* unknown format */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else if(pe == (ps + 4))
            {
               if (*ps == 'r')
               {
                  char s1 = *(ps+1);
                  char s2 = *(ps+2);
                  char s3 = *(ps+3);
                  uint8_t base;

                  if(s1 == '*')
                  {
                     base = (uint8_t) va_arg(val, int);

                     if(base > 36)
                     { /* unsupported base */
                        pCB(pUserData, ps, 0);
                        goto Exit;
                     }
                  }
                  else if (IS_DIGIT(s1))
                  {
                     base = (uint8_t) (s1 - '0');
                  }
                  else
                  { /* unsupported base */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }

                  if(s2 == 'l')
                  {
                     if(s3 == '8')
                     { /* integer of 8 bytes width */
                        CHECK_SIGN (int64_t, uint64_t, int64_t, uint64_t);
                        zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                     }
                     else if(s3 == '4')
                     { /* integer of 4 bytes width */
                        CHECK_SIGN (int32_t, uint32_t, int32_t, uint32_t);
                        zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                     }
                     else if(s3 == '2')
                     { /* integer of 2 bytes width */
                        CHECK_SIGN (int16_t, uint16_t, int, unsigned int);
                        zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                     }
                     else if(s3 == '1')
                     { /* integer of 1 byte width */
                        CHECK_SIGN (int8_t, uint8_t, int, unsigned int);
                        zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                     }
                     else if(s3 == 'l')
                     { /* argument of type long long */
                        CHECK_SIGN (long long, unsigned long long, long long, unsigned long long);
                        zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                     }
                     else
                     { /* unknown format */
                        pCB(pUserData, ps, 0);
                        goto Exit;
                     }
                  }
                  else if((s2 == 'h') && (s3 == 'h'))
                  {
                     CHECK_SIGN (signed char, unsigned char, int, unsigned int);
                     zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                  }
                  else
                  { /* unknown format */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else if(pe == (ps + 5))
            {
               uint8_t base;
               char s0 = *ps;
               char s1 = *(ps+1);
               char s2 = *(ps+2);
               char s3 = *(ps+3);
               char s4 = *(ps+4);

               if(s0 == 'r')
               {
                  if(s1 == '*')
                  {
                     base = (uint8_t) va_arg(val, int);

                     if(base > 36)
                     { /* unsupported base */
                        pCB(pUserData, ps, 0);
                        goto Exit;
                     }
                  }
                  else if (IS_DIGIT(s1))
                  {
                     base = (uint8_t) (s1 - '0');
                  }
                  else
                  { /* unsupported base */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }

                  if (s2 == 'I')
                  {
                     if((s3 == '6') && (s4 == '4'))
                     { /* integer of 8 bytes width */
                        CHECK_SIGN (int64_t, uint64_t, int64_t, uint64_t);
                        zRet += cbk_print_u64(pUserData, pCB, (uint64_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                     }
                     else if((s3 == '3') && (s4 == '2'))
                     { /* integer of 4 bytes width */
                        CHECK_SIGN (int32_t, uint32_t, int32_t, uint32_t);
                        zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                     }
                     else if((s3 == '1') && (s4 == '6'))
                     { /* integer of 2 bytes width */
                        CHECK_SIGN (int16_t, uint16_t, int, unsigned int);
                        zRet += cbk_print_u32(pUserData, pCB, (uint32_t) u, base, sign_char, prefixing, left_justified, blank_padding, precision, minimum_width);
                     }
                     else
                     { /* unknown format */
                        pCB(pUserData, ps, 0);
                        goto Exit;
                     }
                  }
                  else
                  { /* unknown format */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else
            { /* unknown format */
                pCB(pUserData, ps, 0);
                goto Exit;
            }
         }
         else if((fc == 'p') || (fc == 'P'))
         {
            if(pe == ps)
            {
               void * pv = va_arg(val, void *);

               if(sizeof(pv) <= 4)
                  zRet += cbk_print_u32(pUserData, pCB, (uint32_t) (ptrdiff_t) pv, fc, '\0' /*sign_char */, prefixing, left_justified, 1, 8, minimum_width);
               else
                  zRet += cbk_print_u64(pUserData, pCB, (uint64_t) (ptrdiff_t) pv, fc, '\0' /*sign_char */, prefixing, left_justified, 1, 16, minimum_width);
            }
            else
            { /* unknown format */
               pCB(pUserData, ps, 0);
               goto Exit;
            }
         }
         else if(IS_PRINTF_FMT_FLT(fc))
         {
            if(pe == ps)
            {
               double dbl = va_arg(val, double);
               zRet += cbk_print_double(pUserData, pCB, dbl, 10, sign_char, fc, prefixing, left_justified, blank_padding, (precision == ~(size_t) 0) ? 6 : precision, minimum_width);
            }
            else if(pe == (ps + 1)) 
            {
               char s0 = *ps;

               if(s0 == 'L')
               {
                  long double ldbl = va_arg(val, long double);
                  zRet += cbk_print_long_double(pUserData, pCB, ldbl, 10, sign_char, fc, prefixing, left_justified, blank_padding, (precision == ~(size_t) 0) ? 6 : precision, minimum_width);
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else if(pe == (ps + 2))
            {
               char s0 = *ps;
               char s1 = *(ps+1);

               if (s0 == 'r')
               {
                  double  dbl;
                  uint8_t base;

                  if(s1 == '*')
                  {
                     base = (uint8_t) va_arg(val, int);
                     dbl  = va_arg(val, double);

                     if(base > 36)
                     { /* unsupported base */
                        pCB(pUserData, ps, 0);
                        goto Exit;
                     }
                  }
                  else if(IS_DIGIT(s1))
                  {
                     dbl  = va_arg(val, double);
                     base = (uint8_t) (s1 - '0');
                  }
                  else
                  { /* unsupported base */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }

                  if (!base)
                     base = 10;
                  else if (base == 1)
                     base = 16;

                  zRet += cbk_print_double(pUserData, pCB, dbl, base, sign_char, fc, prefixing, left_justified, blank_padding, (precision == ~(size_t) 0) ? 6 : precision, minimum_width);
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else if(pe == (ps + 3))
            {
               char s0 = *ps;
               char s1 = *(ps+1);
               char s2 = *(ps+2);

               if ((s0 == 'r') && (s2 == 'L'))
               {
                  double  long ldbl;
                  uint8_t base;

                  if(s1 == '*')
                  {
                     base = (uint8_t) va_arg(val, int);
                     ldbl = va_arg(val, long double);

                     if(base > 36)
                     { /* unsupported base */
                        pCB(pUserData, ps, 0);
                        goto Exit;
                     }
                  }
                  else if(IS_DIGIT(s1))
                  {
                     ldbl = va_arg(val, long double);
                     base = (uint8_t) (s1 - '0');
                  }
                  else
                  { /* unsupported base */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }

                  if (!base)
                     base = 10;
                  else if (base == 1)
                     base = 16;

                  zRet += cbk_print_long_double(pUserData, pCB, ldbl, base, sign_char, fc, prefixing, left_justified, blank_padding, (precision == ~(size_t) 0) ? 6 : precision, minimum_width);
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else
            { /* unknown format */
               pCB(pUserData, ps, 0);
               goto Exit;
            }
         }
         else if((fc | 0x20) == 'a')
         {
            if(pe == ps)
            {
               double dbl = va_arg(val, double);
               zRet += cbk_print_double(pUserData, pCB, dbl, 2, sign_char, fc, prefixing, left_justified, blank_padding, (precision == ~(size_t) 0) ? (size_t) ((DBL_MANT_DIG + 3) / 4) : precision, minimum_width);
            }
            else if(pe == (ps + 1))
            {
               char s0 = *ps;

               if(s0 == 'L')
               {
                  long double ldbl = va_arg(val, long double);
                  zRet += cbk_print_long_double(pUserData, pCB, ldbl, 2, sign_char, fc, prefixing, left_justified, blank_padding, (precision == ~(size_t) 0) ? (size_t) ((LDBL_MANT_DIG + 3) / 4) : precision, minimum_width);
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else
            { /* unknown format */
               pCB(pUserData, ps, 0);
               goto Exit;
            }
         }
         else if(fc == 'S')
         {
            if(pe == ps)
            {
               wchar_t * pa = va_arg(val, wchar_t *);
               wchar_t * pe = pa;
               size_t length = precision; /* maximum length to be printed */
               if(!pa)
               {
                  pa = (wchar_t *) L"<NULL>";
                  pe = pa;
               }
               while (length-- && *pe)
                  ++pe;

               length = (size_t) (pe - pa); /* contains string len to be printed now */
               zRet += cbk_print_wstring(pUserData, pCB, pa, length, sizeof(wchar_t), minimum_width, left_justified);
            }
            else
            { /* unknown format */
               pCB(pUserData, ps, 0);
               goto Exit;
            }
         }
         else if(fc == 'C')
         {
            if(pe == ps)
            {
               wchar_t wc = (wchar_t) va_arg(val, unsigned int);
               zRet += cbk_print_wstring(pUserData, pCB, &wc, 1, sizeof(wchar_t), minimum_width, left_justified);
            }
            else
            { /* unknown format */
               pCB(pUserData, ps, 0);
               goto Exit;
            }
         }
         else if(fc == 'n')
         {
            if(pe == ps)
            {
               int * pl = va_arg(val, int *);
               *pl = (int) zRet;
            }
            else if(pe == (ps + 1))
            {
               char s0 = *ps;

               if(s0 == 'l')
               {
                  long * pl = va_arg(val, long *);
                  *pl = (long) zRet;
               }
               else if((s0 == 'z') || (s0 == 't') || (s0 == 'I'))
               {
                  ptrdiff_t * pl = va_arg(val, ptrdiff_t *);
                  *pl = (ptrdiff_t) zRet;
               }
               else if(s0 == 'h')
               {
                  short * pl = va_arg(val, short *);
                  *pl = (short) zRet;
               }
               else if(s0 == 'j')
               {
                  intmax_t * pl = va_arg(val, intmax_t *);
                  *pl = (intmax_t) zRet;
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else if(pe == (ps + 2))
            {
               char s0 = *ps;
               char s1 = *(ps+1);

               if(s0 == 'l')
               {
                  if(s1 == '8')
                  { /* integer of 8 bytes width */
                     int64_t * pl = va_arg(val, int64_t *);
                     *pl = (int64_t) zRet;
                  }
                  else if(s1 == '4')
                  { /* integer of 4 bytes width */
                     int32_t * pl = va_arg(val, int32_t *);
                     *pl = (int32_t) zRet;
                  }
                  else if(s1 == '2')
                  { /* integer of 2 bytes width */
                     int16_t * pl = va_arg(val, int16_t *);
                     *pl = (int16_t) zRet;
                  }
                  else if(s1 == '1')
                  { /* integer of 1 byte width */
                     int8_t * pl = va_arg(val, int8_t *);
                     *pl = (int8_t) zRet;
                  }
                  else if(s1 == 'l')
                  {
                     long long * pl = va_arg(val, long long *);
                     *pl = (long long) zRet;
                  }
                  else
                  { /* unknown format */
                     pCB(pUserData, ps, 0);
                     goto Exit;
                  }
               }
               else if((s0 == 'h') && (s1 == 'h'))
               {
                  char * pl = va_arg(val, char *);
                  *pl = (char) zRet;
               }
               else
               { /* unknown format */
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }
            }
            else
            { /* unknown format */
               pCB(pUserData, pe, 0);
               goto Exit;
            }
         }
         else if(fc == 'v')
         {
            if(pe == ps)
            {
               PRINTF_V_CALLBACK * pcbk   = va_arg(val, PRINTF_V_CALLBACK *);
               void *              pvdata = va_arg(val, void *);

               if(!pcbk)
               {
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }

               zRet += pcbk(pUserData, pCB, pvdata, precision, minimum_width, left_justified, prefixing);
            }
            else
            { /* unknown format */
               pCB(pUserData, ps, 0);
               goto Exit;
            }
         }
         else if(fc == 'V')
         {
            if(pe == ps)
            {
               PRINTF_V_DATA * pcd = va_arg(val, PRINTF_V_DATA *);

               if(!pcd || !pcd->pcb)
               {
                  pCB(pUserData, ps, 0);
                  goto Exit;
               }

               zRet += pcd->pcb(pUserData, pCB, pcd, precision, minimum_width, left_justified, prefixing);
            }
            else
            { /* unknown format */
               pCB(pUserData, ps, 0);
               goto Exit;
            }
         }
         else
         { /* unknown format specification -> skip all remaining string data */
            pCB(pUserData, ps, 0);
            goto Exit;
         }

         /* find begin of next format string */
         ps = ++pe;
         while(*pe && (*pe != '%'))
            ++pe;

         if(ps != pe)
         {/* print the intermediate text */
            pCB(pUserData, ps, pe - ps);
            zRet += (size_t)(pe - ps);
         }
         pf = pe; /* end of string or next format string */
      }
   }

   Exit:;
   return (zRet);
} /* size_t cbk_printf(void * pUserData, PRINT_CALL_BACK * pCB, const char * pFmt, va_list val) */



/* ========================================================================= *\
   Implementation of our vsnprintf wrapper
\* ========================================================================= */

/* ------------------------------------------------------------------------- *\
   STRING_WRITE_DATA is our user defined struct that contains the required
   information  of our callback function vsnprintf_write_calback for writing
   the string data and storing the error values
\* ------------------------------------------------------------------------- */

typedef struct STRING_WRITE_DATA_S STRING_WRITE_DATA;
struct STRING_WRITE_DATA_S
{
#ifdef _DEBUG
   char * pSart;   /* start pointer to destination buffer for watching the data changes during debugging */
#endif
   char * pDst;    /* pointer to position in destination buffer for next data */
   size_t DstSize; /* remaining destination buffer size */
   int    Err;     /* error code, EARG or ENOBUF */
};


/* ------------------------------------------------------------------------- *\
   vsnprintf_write_callback is our callback for callback_printf that is used
   by the svsnprintf function
\* ------------------------------------------------------------------------- */

static void vsnprintf_write_callback(void * pUserData, const char * pSrc, size_t Length)
{
   STRING_WRITE_DATA * pwd = (STRING_WRITE_DATA *) pUserData;

   if (!Length)
   { /* callback_printf calls the callback a last time with zero length data in case of any errors within the format string */
      if(!pwd->Err)
         pwd->Err = EINVAL; /* invalid argument detected */
   }
   else
   {
      char * pd = pwd->pDst;

      if(Length > pwd->DstSize)
      {
         if(!pwd->Err)
#ifdef ENOBUFS
            pwd->Err = ENOBUFS; /* buffer size exceeded */
#else
            pwd->Err = ENOMEM; /* buffer size exceeded (ENOBUFS is unknown by old MSVC compilers) */
#endif
         Length = pwd->DstSize;
      }

      pwd->DstSize -= Length;

      while(Length--)
         *pd++ = *pSrc++;

      pwd->pDst = pd;
   }
} /* void  vsnprintf_write_callback(void * pUserData, const char * pSrc, size_t Length) */


/* ------------------------------------------------------------------------- *\
   svsprintf is a wrapper for vsprintf that bases on callback_printf.
\* ------------------------------------------------------------------------- */

size_t svsnprintf(char * pDst, size_t n, const char * pFmt, va_list val)
{
   STRING_WRITE_DATA swd =
   {
#ifdef _DEBUG
      pDst,
#endif
      pDst,
      pDst ? n : 0,
      0
   };

   size_t zRet = callback_printf(&swd, &vsnprintf_write_callback, pFmt, val);

   if(swd.DstSize)
      pDst[zRet] = '\0'; /* add the string terminating character */

   if(swd.Err)
      errno = swd.Err;

   return (zRet);
} /* size_t svsnprintf(char * pDst, size_t n, const char * pFmt, va_list ap) */



/* ------------------------------------------------------------------------- *\
   svsprintf is a wrapper for vsprintf that bases on callback_printf.
\* ------------------------------------------------------------------------- */

size_t svsprintf(char * pDst, const char * pFmt, va_list val)
{
   return (svsnprintf(pDst, ~(size_t) 0, pFmt, val));
} /* size_t svsprintf(char * pDst, const char * pFmt, va_list val) */



/* ------------------------------------------------------------------------- *\
   ssnprintf is a wrapper for snprintf that bases on callback_printf.
\* ------------------------------------------------------------------------- */

size_t ssnprintf(char * pDst, size_t n, const char * pFmt, ...)
{
   size_t zRet;
   va_list VarArgs;
   va_start(VarArgs, pFmt);
   zRet = svsnprintf(pDst, n, pFmt, VarArgs);
   va_end(VarArgs);
   return (zRet);
} /* size_t ssnprintf(char * pDst, size_t n, const char * pFmt, ...) */


/* ------------------------------------------------------------------------- *\
   _ssnprintf is a wrapper for snprintf that bases on callback_printf.
\* ------------------------------------------------------------------------- */

size_t _ssnprintf(char * pDst, size_t n, const char * pFmt, ...)
{
   size_t zRet;
   va_list VarArgs;
   va_start(VarArgs, pFmt);
   zRet = svsnprintf(pDst, n, pFmt, VarArgs);
   va_end(VarArgs);
   return (zRet);
} /* size_t _ssnprintf(char * pDst, size_t n, const char * pFmt, ...) */


/* ------------------------------------------------------------------------- *\
   ssprintf is a wrapper for sprintf that bases on callback_printf.
\* ------------------------------------------------------------------------- */

size_t ssprintf(char * pDst, const char * pFmt, ...)
{
   size_t zRet;
   va_list VarArgs;
   va_start(VarArgs, pFmt);
   zRet = svsnprintf(pDst, ~(size_t) 0, pFmt, VarArgs);
   va_end(VarArgs);
   return (zRet);
} /* size_t  ssprintf(char * pDst, const char * pFmt, ...) */


/* ------------------------------------------------------------------------- *\
   _ssprintf is a wrapper for sprintf that bases on callback_printf.
\* ------------------------------------------------------------------------- */

size_t _ssprintf(char * pDst, const char * pFmt, ...)
{
   size_t zRet;
   va_list VarArgs;
   va_start(VarArgs, pFmt);
   zRet = svsnprintf(pDst, ~(size_t) 0, pFmt, VarArgs);
   va_end(VarArgs);
   return (zRet);
} /* size_t  _ssprintf(char * pDst, const char * pFmt, ...) */


/* ========================================================================= *\
   E N D   O F   F I L E
\* ========================================================================= */
