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

size_t Buffer::Write(const void * d, size_t size)
{
 if (size == 0) {
    return 0;
 }

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

const void * Buffer::GetData(void) const
{
 return data;
}

size_t Buffer::GetSize(void) const
{
 return position;
}

off_t Buffer::Tell(void) const
{
 return position;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
