/* Copyright string in product ressource information */
#define COPYRIGHT_HOLDER "Dipl.-Ing. Klaus Lux (Aachen, Germany)"
#define COPYRIGHT_STRING "\xA9 2024 " COPYRIGHT_HOLDER

/* Generic version number stuff */

/* global define of version number digits */
#define VERSION_1  1
#define VERSION_2  0
#define VERSION_3  0
#define VERSION_4  0

/* helper macros for creating version number strings */
#define MK_STRING(a) #a
#define VERSION_NUMBER_TO_STRING(a,b,c,d)   MK_STRING(a) "." MK_STRING(b) "." MK_STRING(c) "." MK_STRING(d)

/* version number digit list required for resource file, i.e. 1,0,0,1 */
#define VERSION_NUMBER_LIST    VERSION_1,VERSION_2,VERSION_3,VERSION_4

/* version number in form of a single dword */
#define VERSION_NUMBER         ((VERSION_1 << 24) | (VERSION_2 << 16) | (VERSION_3 << 8) | (VERSION_4))

/* version string in form "1.0.0.1" */
#ifdef _DEBUG
#define VERSION_NUMBER_STRING   VERSION_NUMBER_TO_STRING(VERSION_1,VERSION_2,VERSION_3,VERSION_4) " (DEBUG)"
#elif defined (BUILD_VERSION)
/* BUILD_VERSION as set in Makefile */
#define VERSION_NUMBER_STRING   BUILD_VERSION
#else
#define VERSION_NUMBER_STRING   VERSION_NUMBER_TO_STRING(VERSION_1,VERSION_2,VERSION_3,VERSION_4)
#endif

/* ========================================================================= *\
   END OF FILE
\* ========================================================================= */
