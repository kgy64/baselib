/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <Debug/Debug.h>
#include <Exceptions/Exceptions.h>

#include "FileFunctions.h"

SYS_DECLARE_MODULE(DM_FILE);

bool FILES::Remove(const char * p_path)
{
 SYS_DEBUG_FUNCTION(DM_FILE);

 struct stat st;
 int result = stat(p_path, &st);
 if (result < 0) {
    switch (errno) {
        case ENOENT:
            // Does not exist: not an error here
            return false;
        break;
    }
    throw EX::Error() << "Could not stat() '" << p_path << "'";
 }

 result = unlink(p_path);

 ASSERT_DBG(result == 0, "Could not unlink() '" << p_path << "'");

 return true;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
