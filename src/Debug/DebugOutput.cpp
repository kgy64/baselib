/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic Library
 * Purpose:     Default output stream for Debug Logger module
 * Author:      Kövesdi György  (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "DebugOutput.h"

// Putting this function here forces this object to be linked. It is necessary
// because otherwise it contains only singleton implementation, without other
// references. Thsi is a hack.
//
void _Debug_Info_::DebugPrint::endline(void)
{
 GetOutStream() << "\n"; // FIXME: System-specific!
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *         class DefaultDebugOutput:                                                     *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

AUTON_IMPLEMENT(DefaultDebugOutput, I_DebugOut);

I_DebugOut & DefaultDebugOutput::operator<<(const char * msg)
{
 std::cerr << msg;
 return *this;
}

I_DebugOut & DefaultDebugOutput::operator<<(const void * p)
{
 std::cerr << p;
 return *this;
}

I_DebugOut & DefaultDebugOutput::operator<<(int val)
{
 std::cerr << val;
 return *this;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
