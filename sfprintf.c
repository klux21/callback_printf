/*****************************************************************************\
*                                                                             *
*  FILE NAME:     sfprintf.c                                                  *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  DESCRIPTION:   sample fprintf wrapper that uses callback_printf            *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  COPYRIGHT:     (c) 2026 Dipl.-Ing. Klaus Lux (Aachen, Germany)             *
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


#undef   _CRT_ERRNO_DEFINED
#include <errno.h>

#ifdef _WIN32
#include <io.h>       /* write() */
#include <Windows.h>  /* Sleep() */
#pragma warning(disable : 4100 4127 4706 4710)
#else
#include <unistd.h>  /* usleep() */
#endif

#include <sfprintf.h>

/* ========================================================================= *\
   Implementation of our fprintf wrapper
\* ========================================================================= */

/* ------------------------------------------------------------------------- *\
   FILE_WRITE_DATA is our user defined struct that contains the required
   information  of our callback function vfprintf_write_calback for writing
   the string data and storing the error values
\* ------------------------------------------------------------------------- */

typedef struct WRITE_DATA_S WRITE_DATA;
struct WRITE_DATA_S
{
   FILE * pf;     /* file pointer for writing */
   size_t Length; /* successfully written length */
   int    Err;    /* error code, EARG or ENOBUF */
};


/* ------------------------------------------------------------------------- *\
   sfprintf_write_callback is our callback for callback_printf that is used
   by the sfprintf function
\* ------------------------------------------------------------------------- */

static void sfprintf_write_callback(void * pUserData, const char * pSrc, size_t Length)
{
   WRITE_DATA * pwd = (WRITE_DATA *) pUserData;

   if (!Length || pwd->Err)
   { /* callback_printf calls the callback a last time with zero length data in case of any errors within the format string */
      if(!pwd->Err)
         pwd->Err = EINVAL; /* invalid argument detected */
   }
   else
   {
      size_t sz = fwrite(pSrc, 1, Length, pwd->pf); 
      pwd->Length += sz;

      while(sz < Length)
      {
         if (errno == EINTR)
         { /* interrupted by a signal -> just try again */
         }
         else if (errno == EAGAIN)
         {
#ifdef _WIN32
            Sleep(1);
#else
            usleep(1000);
#endif
         }
         else
         { 
            pwd->Err = errno;
            break;
         }

         Length -= sz;
         pSrc   += sz;

         sz = fwrite(pSrc, 1, Length, pwd->pf); 
         pwd->Length += sz;
      }
   }
} /* void  sfprintf_write_callback(void * pUserData, const char * pSrc, size_t Length) */



/* ------------------------------------------------------------------------- *\
   svfprintf is a wrapper for vfprintf that bases on callback_printf.
\* ------------------------------------------------------------------------- */

size_t svfprintf(FILE * pf, const char * pFmt, va_list val)
{
   size_t sz_ret = 0;

   if(!pf || !pFmt)
   {
      errno = EINVAL;
   }
   else
   {
      WRITE_DATA wd =
      {
         pf,
         0,
         0
      };

      callback_printf(&wd, &sfprintf_write_callback, pFmt, val);

      sz_ret = wd.Length;

      if(wd.Err)
         errno = wd.Err;
   }

   return (sz_ret);
} /* size_t svfprintf(FILE * pf, const char * pFmt, va_list val) */



/* ------------------------------------------------------------------------- *\
   sfprintf is a wrapper for fprintf that bases on callback_printf.
\* ------------------------------------------------------------------------- */

size_t sfprintf(FILE * pf, const char * pFmt, ...)
{
   size_t sz_ret = 0;

   va_list val;
   va_start(val, pFmt);

   sz_ret =  svfprintf(pf, pFmt, val);

   va_end(val);

   return (sz_ret);
} /* size_t sfprintf(FILE * pf, const char * pFmt, ...) */


/* ------------------------------------------------------------------------- *\
   _sfprintf is a wrapper for fprintf that bases on callback_printf.
\* ------------------------------------------------------------------------- */

size_t _sfprintf(FILE * pf, const char * pFmt, ...)
{
   size_t sz_ret = 0;

   va_list val;
   va_start(val, pFmt);

   sz_ret =  svfprintf(pf, pFmt, val);

   va_end(val);

   return (sz_ret);
} /* size_t _sfprintf(FILE * pf, const char * pFmt, ...) */



/* ========================================================================= *\
   Implementation of sfdprintf 
\* ========================================================================= */


typedef struct FD_WRITE_DATA_S FD_WRITE_DATA;
struct FD_WRITE_DATA_S
{
   int    fd;     /* file descriptor to write to */
   size_t Length; /* successfully written length */
   int    Err;    /* error code, EARG or ENOBUF */
};


/* ------------------------------------------------------------------------- *\
   sfdprintf_write_callback is our callback for callback_printf that is used
   by the sfdprintf function
\* ------------------------------------------------------------------------- */

static void sfdprintf_write_callback(void * pUserData, const char * pSrc, size_t Length)
{
   FD_WRITE_DATA * pwd = (FD_WRITE_DATA *) pUserData;

   if (!Length || pwd->Err)
   { /* callback_printf calls the callback a last time with zero length data in case of any errors within the format string */
      if(!pwd->Err)
         pwd->Err = EINVAL; /* invalid argument detected */
   }
   else
   {
      ptrdiff_t sz = write(pwd->fd, pSrc, Length > 0x20000 ? 0x20000 : (unsigned int) Length); 

      if(sz > 0)
      {
         pwd->Length += sz;
         Length -= sz;
         pSrc   += sz;
      }

      while(Length)
      {
         if(sz < 0)
         {
            if (errno == EINTR)
            { /* interrupted by a signal -> just try again */
            }
#ifdef _WIN32
            else if (errno == EAGAIN)
            {
               Sleep(1);
            }
#else
            else if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
               usleep(1000);
            }
#endif
            else
            { 
               pwd->Err = errno;
               break;
            }
         }

         sz = write(pwd->fd, pSrc, Length > 0x20000 ? 0x20000 : (unsigned int) Length);
         if(sz > 0)
         {
            pwd->Length += sz;
            Length -= sz;
            pSrc   += sz;
         }
      }
   }
} /* void  sfdprintf_write_callback(void * pUserData, const char * pSrc, size_t Length) */


/* ------------------------------------------------------------------------- *\
   svfdprintf is a vfprintf like function that bases on callback_printf but
   writes to a file desciptor.
\* ------------------------------------------------------------------------- */

size_t svfdprintf(int fd, const char * pFmt, va_list val)
{
   size_t sz_ret = 0;

   if((fd == -1) || !pFmt)
   {
      errno = EINVAL;
   }
   else
   {
      FD_WRITE_DATA wd =
      {
         fd,
         0,
         0
      };

      callback_printf(&wd, &sfdprintf_write_callback, pFmt, val);

      sz_ret = wd.Length;

      if(wd.Err)
         errno = wd.Err;
   }

   return (sz_ret);
} /* size_t svfdprintf(FILE * pf, const char * pFmt, va_list val) */


/* ------------------------------------------------------------------------- *\
   sfdprintf is a fprintf like function that bases on callback_printf but
   writes to a file desciptor.
\* ------------------------------------------------------------------------- */

size_t sfdprintf(int fd, const char * pFmt, ...)
{
   size_t sz_ret = 0;

   va_list val;
   va_start(val, pFmt);

   sz_ret = svfdprintf(fd, pFmt, val);

   va_end(val);

   return (sz_ret);
} /* size_t sfdprintf(FILE * pf, const char * pFmt, ...) */

/* ------------------------------------------------------------------------- *\
   _fdprintf is a fprintf like function that bases on callback_printf but
   writes to a file desciptor.
\* ------------------------------------------------------------------------- */

size_t _fdprintf(int fd, const char * pFmt, ...)
{
   size_t sz_ret = 0;

   va_list val;
   va_start(val, pFmt);

   sz_ret = svfdprintf(fd, pFmt, val);

   va_end(val);

   return (sz_ret);
} /* size_t _fdprintf(FILE * pf, const char * pFmt, ...) */


/* ========================================================================= *\
   E N D   O F   F I L E
\* ========================================================================= */
