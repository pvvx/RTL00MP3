/**************************************************
 * malloc/free/realloc wrap for gcc compiler
 *
 **************************************************/
#if defined(__GNUC__)
#include "FreeRTOS.h"

void* __wrap_malloc( size_t size )
{
    return pvPortMalloc(size);
}

void* __wrap_realloc( void *p, size_t size )
{
    return pvPortReAalloc(p, size);
}

void __wrap_free( void *p )
{
    vPortFree(p);
}
#endif