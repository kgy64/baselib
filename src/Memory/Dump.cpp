/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Dump.h"

using namespace Memory;

void Dump::PrintHex(std::ostream & os, uint64_t p_value, int p_size)
{
 char tmp[20];
 const char * format = NULL;

 switch (p_size) {
    case 1:
        format = "%02lx";
        p_value &= 0xff;
    break;
    case 2:
        format = "%04lx";
        p_value &= 0xffff;
    break;
    case 4:
        format = "%08lx";
        p_value &= 0xffffffff;
    break;
    case 8:
        format = "%016lx";
    break;
 }

 if (!format) {
    return;
 }

 sprintf(tmp, format, p_value);

 os << tmp;
}

void Dump::ToStream(std::ostream & os)
{
 os << "Memory dump from " << std::hex << (void*)myStartAddress << " follows:";
 while (mySize > 0) {
    os << std::endl;
    int offset = ((int64_t)myStartAddress) & 0x0f;
    const unsigned char * real_start = myStartAddress - offset;
    Print(os, real_start);
    os << ":";
    for (int i = 0; i < 16; ++i) {
        if ((i&3)==0) {
            os << " ";
        }
        int index = i - offset;
        if (index >= 0 && index < mySize) {
            Print(os, myStartAddress[index]);
            os << " ";
        } else {
            os << "   ";
        }
    }
    os << "|";
    for (int i = 0; i < 16; ++i) {
        if ((i&3)==0 && i) {
            os << " ";
        }
        int index = i - offset;
        if (index >= 0 && index < mySize) {
            os << (char)((myStartAddress[index] < ' ' || myStartAddress[index] > 0x7e) ? '.' : myStartAddress[index]);
        } else {
            os << " ";
        }
    }
    os << "|";
    myStartAddress += 16 - offset;
    mySize -= 16 - offset;
 }
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
