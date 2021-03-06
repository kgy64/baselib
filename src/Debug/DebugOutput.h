/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic Library
 * Purpose:     Default output stream for Debug Logger module
 * Author:      Kövesdi György  (kgy@etiner.hu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_SRC_DEBUG_DEBUGOUTPUT_H_INCLUDED__
#define __BASELIB_SRC_DEBUG_DEBUGOUTPUT_H_INCLUDED__

class DefaultDebugOutput: public I_DebugOut
{
 private:
    virtual ~DefaultDebugOutput()
    {
    }

    virtual I_DebugOut & operator<<(const char * msg);

}; // DefaultDebugOutput

#endif /* __BASELIB_SRC_DEBUG_DEBUGOUTPUT_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
