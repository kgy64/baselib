/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Memory buffer for temporary usage
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "TemporaryBuffer.h"

#include <stdlib.h>

using namespace FILES;

Buffer::Buffer(void):
    position(0),
    allocated(65536),
    data(malloc(allocated))
{
 ASSERT(data, "Memory allocation problem");
}

Buffer::~Buffer()
{
 free(data);
}

uint32_t Buffer::Write(void * d, uint32_t size)
{
 if (position + size > allocated) {
    do {
        allocated <<= 1;
    } while (position + size > allocated);
    data = realloc(data, allocated);
    ASSERT(data, "Memory reallocation problem");
 }

 memcpy(reinterpret_cast<char*>(data) + position, d, size);
 position += size;

 return size;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
