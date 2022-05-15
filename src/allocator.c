#include "allocator.h"

/*
 * small chunk are array like , every small chunk gets 2k bytes and 
 * there are total 126 chunks vary from 8byte to 1024byte  
 * At the same time the array struct contains a size_t and word_t where store data exactly
 * 
 * so every array size is 8+chunk size;
 * for 16byte array
 * Assume the start array at 0x00000000 the next should start at 0x00000018 
 * 16 in hex is 0x10 in bin is 0b10000 the last 4 bit is not used.
 * 
 * So the last four bit is control bit 
 * 
 * -------------------------
 * |  0  |  0  |  0  |  1  |
 * -------------------------
 *               prev   rb
 * 
 * 
 * represents in using.
 * 
 */

word_t* alloc_t(size_t size, Block_t*(*func)(size_t))
{
  if(Heap == NULL)
  {
    /* Check the pointer address before continue */
    Heap = sbrk(0);
    printf("Heap:          %p\n",Heap);
    sbrk(align(allocSizeHeap(HEAP_MIN)));
    Heap->allocated = (void*)Heap + sizeof(Heap_t);
    printf("allocated:     %p\n",Heap->allocated);
  }
  if(Heap->size < size)
  {
    sbrk(3*HEAP_MIN);
  }

  word_t* data = NULL;

  if(size < THRESHOLD_ARRAY)
  {
    /* find chunk in the array */
    unsigned char index = find_array(size);
    data = find_array_chunk(index);
    if(data == NULL)
    {
      /* process there is no small area */
    }
  }
  else
  {
    // printf("Find a block\n");
    /* find chunk in the linked-list */
    data = find_block(size);
  }


  return data;
}


word_t* alloc(size_t size)
{
  return alloc_t(size,NULL);
}

