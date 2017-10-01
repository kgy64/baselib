/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Baselib - Basic System Access Library
 * Purpose:     
 * Author:      György Kövesdi <kgy@etiner.hu>
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_OPSYS_UNIX_FILE_PATHHANDLER_H_INCLUDED__
#define __BASELIB_OPSYS_UNIX_FILE_PATHHANDLER_H_INCLUDED__

#include <Base/Parser.h>
#include <Debug/Debug.h>
#include <Exceptions/Exceptions.h>

#include <string>

namespace FILES
{
    class PathHandler
    {
     public:
        PathHandler(const char * path);

        inline PathHandler(const std::string & path):
            PathHandler(path.c_str())
        {
        }

        virtual ~PathHandler()
        {
        }

        inline size_t size(void) const
        {
            return chunks.size();
        }

        inline const char * operator[](unsigned p_index) const
        {
            ASSERT(p_index < size(), "access beyond limits: " << p_index << ", size=" << size());
            return chunks[p_index];
        }

        inline const char * GetFilename(void) const
        {
            return filename;
        }

        std::string GetDirectoryPath(void) const;
        std::string GetFullPath(void) const;

     protected:
        Parser::Tokenizer tags;

        std::vector<const char *> chunks;

        const char * filename;

     private:
        SYS_DEFINE_CLASS_NAME("FILES::PathHandler");

    }; // class PathHandler

} // namespace FILES

#endif /* __BASELIB_OPSYS_UNIX_FILE_PATHHANDLER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
