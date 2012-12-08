/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "ICExceptions.h"

using namespace EX;

BaseException::BaseException(const char * msg, int line)
{
 os += msg;
 if (line > 0) {
    os += " at line ";
    os += line;
 }
 os += ": ";
}

BaseException::~BaseException() throw()
{
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
