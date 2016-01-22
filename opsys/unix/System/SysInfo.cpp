/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "SysInfo.h"

#include <File/FileHandler.h>
#include <Base/Parser.h>

#include <string.h>

using SYS::MemInfo;

uint64_t MemInfo::getPhisicalMemorySize(void)
{
 updateInfo();

 for (unsigned i = 0; i < info.size(); ++i) {
    Parser::Tokenizer tok(info[i].c_str(), ": \t");
    if (tok.size() == 0) {
        continue;
    }
    if (strcmp(tok[0], "MemTotal")) {
        continue;
    }
    int64_t result = Parser::StrtollSafe(tok[1]);
    if (tok.size() > 2) {
        switch (tok[2][0]) {
            case 'g':
            case 'G':
                result *= 1024LL;
            case 'm':
            case 'M':
                result *= 1024LL;
            case 'k':
            case 'K':
                result *= 1024LL;
            break;
        }
    }
    return result;
 }

 return 0;
}

uint64_t MemInfo::getMemoryHugePageSize(void)
{
 updateInfo();

 // TODO!
}

void MemInfo::updateInfo(void)
{
 if (!info.empty()) {
    return;
 }

 char tmp[10000];
 memset(tmp, 0, sizeof tmp);

 FILES::FileHandler mem_status("/proc/meminfo");
 mem_status.Open();
 try {
    mem_status.Read(tmp, sizeof tmp);
 } catch (EX::File_EOF & ex) {
 }

 Parser::Tokenizer tok(tmp, "\r\n");

 for (int i = 0; i < tok.size(); ++i) {
    info.push_back(tok[i]);
 }
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
