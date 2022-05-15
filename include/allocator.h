#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <sys/mman.h>
#include "utils.h"
#include "fit.h"


word_t* alloc_t(size_t size, Block_t*(*func)(size_t));
word_t* alloc(size_t size);



#endif