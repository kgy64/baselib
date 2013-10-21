/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic Library
 * Purpose:     Default output stream for Debug Logger module
 * Author:      Kövesdi György  (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_SRC_DEBUG_DEBUGOUTPUT_H_INCLUDED__
#define __BASELIB_SRC_DEBUG_DEBUGOUTPUT_H_INCLUDED__

#include <Debug/Debug.h>

class DefaultDebugOutput: public I_DebugOut
{
 private:
    virtual ~DefaultDebugOutput()
    {
    }

    virtual I_DebugOut & operator<<(const char * msg);
    virtual I_DebugOut & operator<<(const void * p);
    virtual I_DebugOut & operator<<(int val);

}; // DefaultDebugOutput

#endif /* __BASELIB_SRC_DEBUG_DEBUGOUTPUT_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
