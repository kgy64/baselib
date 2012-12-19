/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __INCLUDE_PUBLIC_DEBUG_DEBUGLEVELS_H_INCLUDED__
#define __INCLUDE_PUBLIC_DEBUG_DEBUGLEVELS_H_INCLUDED__

enum _GenericDebugLevels
{
    DL_NONE         =   0x00000000,
    DL_CALLS        =   0x00000001,
    DL_FATAL        =   0x00000002,
    DL_ERROR        =   0x00000004,
    DL_WARNING      =   0x00000008,
    DL_INFO1        =   0x00000010,
    DL_INFO2        =   0x00000020,
    DL_INFO3        =   0x00000040,
    DL_VERBOSE      =   0x00000080,
    DL_NOTFOUND     =   0x00000100,
    DL_NOTUSED      =   0x00000200,
    DL_ALL          =   0xffffffff
};

enum _MixedDebuglevels
{
    DL_GENERIC      =   DL_FATAL | DL_ERROR | DL_WARNING,
    DL_INFO         =   DL_INFO1 | DL_INFO2 | DL_INFO3,
};

#endif /* __INCLUDE_PUBLIC_DEBUG_DEBUGLEVELS_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
