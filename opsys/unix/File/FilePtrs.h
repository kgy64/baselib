/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __OPSYS_UNIX_FILE_FILEPTRS_H_INCLUDED__
#define __OPSYS_UNIX_FILE_FILEPTRS_H_INCLUDED__

#include <boost/shared_ptr.hpp>

namespace FILES
{
    class FileHandler;
    class DirHandler;
    class FileMap;

    typedef boost::shared_ptr<FileHandler> FilePtr;
    typedef boost::shared_ptr<FileMap> MapPtr;
    typedef boost::shared_ptr<DirHandler> DirPtr;
}

#endif /* __OPSYS_UNIX_FILE_FILEPTRS_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
