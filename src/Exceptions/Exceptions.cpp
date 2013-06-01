/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Exceptions.h"

using namespace EX;

BaseException::BaseException(const char * msg, int line)
{
 *this << msg;
 if (line > 0) {
    *this << " at line ";
    *this << line;
 }
 *this << ": ";
}

BaseException::~BaseException() throw()
{
}

#ifdef BOOST_ENABLE_ASSERT_HANDLER
namespace boost
{
    /// Called by boost BOOST_ASSERT() and BOOST_ASSERT_MSG() macros
    /*! \note   The compiler flag BOOST_ENABLE_ASSERT_HANDLER must be defined for this function to be called,
                or else the boost assertion handler will terminate the operation.
     */
    void assertion_failed(char const * expr, char const * function, char const * file, long line)
    {
        throw EX::Assert() << "boost: '" << expr << "' is failed in " << function << "; " << file << ":" << line;
    }
}
#endif

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
