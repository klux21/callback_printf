/*****************************************************************************\
*                                                                             *
*  FILE NAME:     callback_printf.h                                           *
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


#ifndef CALLBACK_PRINTF_H
#define CALLBACK_PRINTF_H

#include <stddef.h>
#include <stdarg.h>
#include <inttypes.h> /* a wrapper for Microsoft Windows can be found in the Win32 subdirectory */

#ifndef PRINTF_LIKE_ARGS
    #if defined(__GNUC__) && __GNUC__
        #define PRINTF_LIKE_ARGS( fmtidx, argidx )        __attribute__(( format( printf, fmtidx, argidx ) ))
    #else
        #define PRINTF_LIKE_ARGS( fmtidx, argidx )        /* as nothing */
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern const char *  LowerDigit;      /* for encoding the digits of bases above 10 in lower letters */
extern const char *  UpperDigit;      /* for encoding the digits of bases above 10 in upper letters */
extern const uint8_t CharType[256];   /* array of character type flags */

#define IS_ASCII(x) (CharType[(uint8_t) (x)] & (uint8_t) 0x01)  /* characters: A-Z a-z _ */
#define IS_DIGIT(x) (CharType[(uint8_t) (x)] & (uint8_t) 0x02)  /* decimal digits: 0 - 9 */
#define IS_ALPHA(x) (CharType[(uint8_t) (x)] & (uint8_t) 0x03)  /* alpha numeric characters */
#define IS_BLANK(x) (CharType[(uint8_t) (x)] & (uint8_t) 0x04)  /* blanks and control characters */
#define IS_OP(x)    (CharType[(uint8_t) (x)] & (uint8_t) 0x08)  /* operators: \n \r ! " # $ % & ( ) * + , - . /  : ; < = > ? [ \ ] ^ @ { | } ~  */

#define IS_PRINTF_FMT_END(x)  (CharType[(uint8_t) (x)] & (uint8_t) 0x10)  /* termination character of a printf format sequence bBdiouxXaAeEfFgGsScCpPnvV% '\0' */
#define IS_PRINTF_FMT_FLAG(x) (CharType[(uint8_t) (x)] & (uint8_t) 0x20)  /* character is a printf format flag ( -+# 0 ) */
#define IS_PRINTF_FMT_INT(x)  (CharType[(uint8_t) (x)] & (uint8_t) 0x40)  /* character terminates a printf integer format string ( bBdiouxX ) */
#define IS_PRINTF_FMT_FLT(x)  (CharType[(uint8_t) (x)] & (uint8_t) 0x80)  /* character terminates the printf floating point formats strings ( eEfFgG ) */

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
int iUtf8Decode (void **  pDst,          /* destination buffer */
                 size_t * pDstSize,      /* remaining destination buffer size */
                 void **  pSrc,          /* source buffer */
                 size_t * pSrcSize,      /* remaining source data size */
                 size_t   DstCharWidth); /* width of destination characters */

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
int iUtf8Encode (void **  pDst,          /* destination buffer */
                 size_t * pDstSize,      /* remaining destination buffer size */
                 void **  pSrc,          /* source buffer */
                 size_t * pSrcSize,      /* remaining source data size */
                 size_t   SrcCharWidth); /* width of source characters */

/* ------------------------------------------------------------------------- *\
   svsnprintf, svsprintf, svsnprintf and ssprintf are system independent
   wrappers for vsnprintf, vsprintf, snprintf and sprintf which are using
   callback_printf internally.
   They are using (v)sprintf like arguments and format strings.
\* ------------------------------------------------------------------------- */
size_t svsnprintf (char * pDst, size_t n, const char * pFmt, va_list ap);
size_t svsprintf  (char * pDst, const char * pFmt, va_list ap);
size_t ssnprintf  (char * pDst, size_t n, const char * pFmt, ...) PRINTF_LIKE_ARGS (3, 4); /* expects a printf like format string and arguments */
size_t ssprintf   (char * pDst, const char * pFmt, ...) PRINTF_LIKE_ARGS (2, 3);           /* expects a printf like format string and arguments */

size_t _ssnprintf (char * pDst, size_t n, const char * pFmt, ...);                         /* same as ssnprintf but without argument type check and possibly unsafe */
size_t _ssprintf  (char * pDst, const char * pFmt, ...);                                   /* same as ssprintf but without argument type check and possibly unsafe */

/* ------------------------------------------------------------------------- *\
  User defined callback function for callback_printf for writing the output
  data. If length is zero then there is an error in format string and pSrc
  points to the problematic position in format string if it is not NULL.
\* ------------------------------------------------------------------------- */
typedef void (PRINTF_CALLBACK) (void * pUserData, const char * pSrc, size_t Length);

/* ------------------------------------------------------------------------- *\
   callback_printf generates vsnprintf like character output by calling a
   user defined write callback for the parts of the generated character data.
   The function returns the length of the overall written data but does not
   terminate the written output data string.
   See implementation of svsnprintf implementation for a sample of usage.
\* ------------------------------------------------------------------------- */
size_t callback_printf (void * pUserData, PRINTF_CALLBACK * pCB, const char * pFmt, va_list val);


/* ========================================================================= *\
   User defined output function for %v or %V option
\* ========================================================================= */

/* ------------------------------------------------------------------------- *\
   cbk_print_string prints a unterminated string of given length using the
   related callback function and returns the written string data length.
   It may simplify the implementation of a PRINTF_V_CALLBACK that creates
   a user data related string of a random length.
\* ------------------------------------------------------------------------- */
size_t cbk_print_string (void *            pUserData,        /* user specific context for the callback */
                         PRINTF_CALLBACK * pCB,              /* data write callback for printing the data */
                         const char *      ps,               /* pointer to string data */
                         size_t            length,           /* length of string data */
                         size_t            minimum_width,    /* minimum width of output */
                         uint8_t           left_justified);  /* whether the output should be right or left justified */

/* ------------------------------------------------------------------------- *\
   Callback function prototype for printing the data of a %v or %V option.
\* ------------------------------------------------------------------------- */
typedef size_t (PRINTF_V_CALLBACK)(void *            pUserData,      /* user specific context for the callback */
                                   PRINTF_CALLBACK * pCB,            /* data write callback for printing the data */
                                   void *            pvdata,         /* user data pointer in second argument for %v or pointer to PRINTF_CALLBACK_DATA in case of %V */
                                   size_t            precision,      /* precision of output (if specified otherwise ~(size_t) 0) */
                                   size_t            minimum_width,  /* minimum width of the output */
                                   uint8_t           left_justified, /* whether the output needs to be left justified */
                                   uint8_t           prefixing);     /* whether the prefixing flag # was specified */

/* ------------------------------------------------------------------------- *\
   PRINTF_V_DATA is a little struct that is required as argument in case of
   a %V option in the format string.
\* ------------------------------------------------------------------------- */
typedef struct PRINTF_V_DATA_S PRINTF_V_DATA;
struct PRINTF_V_DATA_S
{
   PRINTF_V_CALLBACK * pcb;   /* output function callback */
   ptrdiff_t           size;  /* user defined integer e.g. data length or index */
   void *              pdata; /* pointer to user defined data for output */
};

#if 0
   /* Usage sample of user defined output using the %V and %v format options. */
   #include <stdio.h>
   #include <callback_printf.h>

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
      PRINTF_V_DATA * pvd = (PRINTF_V_DATA *) pv_data;

      size_t length = ssnprintf(buffer, sizeof(buffer), "%.*s", (int) (precision <= pvd->size ? precision : pvd->size), (char *) pvd->pdata);

      return ( cbk_print_string( pUserData, pCB, buffer, length, minimum_width, left_justified));
   } /* size_t cbfunc(...) */

   int main()
   {
      PRINTF_V_DATA cbdata = { &cbfunc, 12, (void *) "Hello world!"}; /* output data for %v and %V samples */
      char buf[100];

      ssprintf("%%v: %v\n"   "%%#-12.5v: %#-12.5v\n"   "%%V: %V\n"  "%%*.*V: %*.*V\n",
               &cbfunc, &cbdata,            // %v expects a callback function pointer and a void * pointer as arguments 
               &cbfunc, &cbdata,
               &cbdata,                     // %V expects a pointer to a struct PRINTF_V_DATA as argument
               (int) 12, (int) 3, &cbdata);

      printf("%s", buf);
      return (0);
   } /* int main() */
#endif


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif /* CALLBACK_PRINTF_H */

/* ========================================================================= *\
   E N D   O F   F I L E
\* ========================================================================= */
