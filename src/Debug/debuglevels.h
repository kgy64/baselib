/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@etiner.hu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __INCLUDE_PUBLIC_DEBUG_DEBUGLEVELS_H_INCLUDED__
#define __INCLUDE_PUBLIC_DEBUG_DEBUGLEVELS_H_INCLUDED__

enum _GenericDebugLevels
{
    /// All debug messages are switched off
    DL_NONE         =   0x00000000,

    /// The function call trace is switched on
    /*! Switching on this flag turns the output into different format: a
        semi-graphical representation of the function calls are displayed
        in text format. */
    DL_CALLS        =   0x00000001,

    /// Fatal error
    DL_FATAL        =   0x00000002,

    /// Generic error
    DL_ERROR        =   0x00000004,

    /// Warning message
    DL_WARNING      =   0x00000008,

    /// Important information
    DL_INFO1        =   0x00000010,

    /// Less important information
    DL_INFO2        =   0x00000020,

    /// Not important information
    DL_INFO3        =   0x00000040,

    /// Very detailed information
    /*! \note   Setting this flag probably makes the output very noisy. */
    DL_VERBOSE      =   0x00000080,

    /// Specific message: something is not found
    DL_NOTFOUND     =   0x00000100,

    /// Specific message: something is not used
    DL_NOTUSED      =   0x00000200,

    /// Messages generated by \ref ASSERT_DBG
    DL_ASSERT       =   0x00000400,

    /// User defined message #1
    /*! \note   Such a message is intended to be used for debugging, and not
                recommended to leave such messages in the production code. */
    DL_USER1        =   0x00000800,

    /// User defined message #2
    /*! \note   Such a message is intended to be used for debugging, and not
                recommended to leave such messages in the production code. */
    DL_USER2        =   0x00001000,

    /// User defined message #3
    /*! \note   Such a message is intended to be used for debugging, and not
                recommended to leave such messages in the production code. */
    DL_USER3        =   0x00002000,
};

enum _MixedDebuglevels
{
    /// Commonly used messages
    DL_GENERIC      =   DL_FATAL | DL_ERROR | DL_WARNING,

    /// Important informative messages
    DL_INFO         =   DL_INFO1 | DL_INFO2 | DL_INFO3,

    /// All messages are on
    DL_ALL          =   0xffffffff,
};

#endif /* __INCLUDE_PUBLIC_DEBUG_DEBUGLEVELS_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
