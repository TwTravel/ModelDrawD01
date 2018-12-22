/*
 * Memory allocator for TinyGL
 */
#include "zgl.h"
#include <string.h>
/* modify these functions so that they suit your needs */

void gl_free(void *p)
{
    free(p);
}

void *gl_malloc(int size)
{
	void * pt = malloc(size);
	memset(pt, 0, size);
    return pt;//malloc(size);
}

void *gl_zalloc(int size)
{
    return calloc(1, size);
}
