#include "gc.h"

/* Use double linked list to contain the Block list */
void mfree(void* data)
{
  /* Check if its array address */
  data = check_array_addr(data);
  if(data == NULL)
  {
    return;
  }
  /* Block free part */
}

void* check_array_addr(void* data)
{
  for(size_t i=0;i<16;i++)
  {
    size_t length = (i+1) << 9;
    void* cut_off = (void*)Heap->array_Chunk[i] + sizeof(size_t) + length;
    if((void*)Heap->array_Chunk[i] < data && cut_off > data)
    {
      if((void*)Heap->array_Chunk[i] + sizeof(size_t) == data)
      {
        /* the first needs to be free */
        Heap->array_Chunk[i]->bitmap -= 1;
        data = NULL;
        return NULL;
      }
      size_t size = (i+1)<<3;
      unsigned char free_array = (data - (void*)Heap->array_Chunk[i] + sizeof(size_t)) / size;
      size_t mask = 1;
      mask = mask << free_array;
      mask = ~(mask);
      Heap->array_Chunk[i]->bitmap &= mask;
      data = NULL;
      return NULL;
    }
  }

  return data;
}
