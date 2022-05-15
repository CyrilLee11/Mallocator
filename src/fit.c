#include "fit.h"

unsigned char find_array(size_t size)
{
  size = size < 8 ? 8 : size;
  size_t align_size = align(size);

  return (align_size / 8) - 1;
}

unsigned char check_bitmap(Array_t* array_chunk)
{
  size_t size = array_chunk->bitmap;
  
  unsigned char flag = 0;
  size_t pos = 1;
  while(flag < 64)
  {
    if(size & (pos<<flag))
    {
      flag++;
      continue;
    }
    break;
  }
  
  pos = pos << flag;
  size = size | pos;
  array_chunk->bitmap = size;
  
  return flag;
}

void* find_array_chunk(unsigned char index)
{
  /* check validation */
  if(Heap->array_Chunk[index] == NULL)
  {
    /* create a new array_Chunk */
    /* Need (index + 2) * 8 * 64 >> (index>>1) */
    size_t array_size = align((((index + 1) * 8 ) << 6) + sizeof(size_t));
    Heap->array_Chunk[index] = Heap->allocated;
    Heap->allocated = (void*)Heap->allocated + array_size;

    Heap->array_Chunk[index]->bitmap ^= 0b1;

    return (void*)Heap->array_Chunk[index] + sizeof(size_t);
  }
  else
  {
    /* use bitmap to find the free block */
    if(Heap->array_Chunk[index]->bitmap == 0xffffffffffffffff)
    {
      printf("NULL Reached\n");
      return NULL;
    }
    
    unsigned char flag = check_bitmap(Heap->array_Chunk[index]);
    return (void*)Heap->array_Chunk[index] + sizeof(size_t) + flag * ((index + 1) * 8);
  }
  /* Should be error */
  return NULL;
}

/* Find a Vlock from the Chunk to fit the size */
void* find_block(size_t size)
{
  /* 8k 32k 64k 128k */
  Block_t* get_block;
  Chunk_t* temp = NULL;

  if(size < MINI)
  {
    temp = Heap->linked_Chunk[0];
  }
  else if(size < SMALL)
  {
    temp = Heap->linked_Chunk[1];
  }
  else if(size < MEDIUM)
  {
    temp = Heap->linked_Chunk[2];
  }
  else
  {
    temp = Heap->linked_Chunk[3];
  }

  while(temp && temp->size < size)
  {
    // printf("temp size : %zu\n",temp->size);
    temp = temp->next;
  }
  /* No Chunk can be used so create a new chunk */
  if(temp == NULL)
  {
    temp = create_chunk(MINI);
    /* Allocate memory in the temp chunk */
    if(temp == NULL)
    {
      /* not enough space */
      /* error occured */
      return NULL;
    }
    Block_t* new_block = create_first_block(temp,size);
    // printf("add a first chunk\n");
    // printf("first chunk addr:  %p\n",temp);
    // printf("sizeof chunk    :  %zu\n",sizeof(Chunk_t));
    // printf("first chunk size:  %zu\n",temp->size);
    Heap->linked_Chunk[0] = add_chunk(Heap->linked_Chunk[0],temp);
    /* add the chunk to the chunk list */

    return (void*)new_block + sizeof(Block_t) - 8;
  }
  // printf("Exist \n");
  // printf(“)
  /* First Chunk has been created */
  /* check if the chunk still has enough space */
  /* If not, create a chunk and attach it to the linked list */
  get_block = best_fit_block(size,temp->Head);
  Delete_Block_rb(temp->Head,get_block);
  if(get_block->size - size - sizeof(Block_t) > 0)
  {
    /* Large Block needs to be cut */
    Block_t* small_block = (void*)get_block + size + sizeof(Block_t) - 8;
    small_block->size = get_block->size - size - sizeof(Block_t) + 8;
    get_block->size = size;
    temp->size -= (size + sizeof(Block_t) - 8);
    printf("temp->size : %zu\n",get_block->size);
    Add_Block_rb(temp->Head,small_block);
  }
  else
  {
    temp->size -= get_block->size;
  }
  
  /* update the chunk in the chunk list */
  Heap->linked_Chunk[0] = sort_chunk(Heap->linked_Chunk[0],temp);

  return (void*)get_block + sizeof(Block_t) - 8;
  
}

Block_t* best_fit_block(size_t size,Block_t* block)
{
  /* (pseudo)best fit */
  return Find_Block_rb(block,size);
}

Block_t* create_first_block(Chunk_t* temp, size_t size)
{
  size_t allocated_size = align(sizeof(Block_t) + size);
  Block_t* new_block = (void*)temp + sizeof(Chunk_t);
  new_block->left = NULL;
  new_block->right = NULL;
  new_block->prev_size = 0;
  new_block->size = size;
  new_block->size += 2;
  /* new_block will be returned We need to set up a new block */
  temp->size -= allocated_size;
  if(temp->size > sizeof(Block_t)-8)
  {
    
    Block_t* next_block = (void*)new_block + sizeof(Block_t) + size-8;
    next_block->left = NULL;
    next_block->right = NULL;
    next_block->prev_size = size;
    next_block->size = temp->size + 2; /*10 prev is using; cur unusing*/
    temp->Head = next_block;
    // printf("prev size  : %zu\n",size);
    // printf("next block : %p\n",next_block);
  }
  else
  {
    new_block->size += temp->size;
    temp->size = 0;
  }

  return new_block;
}

Chunk_t* add_chunk(Chunk_t* start_chunk,Chunk_t* temp_chunk)
{
  if(start_chunk == NULL) return temp_chunk;
  /* Some problems might occured  DOUBLE CHECK! */
  if(start_chunk->size < temp_chunk->size)
  {
    temp_chunk->next = start_chunk;
    start_chunk = temp_chunk;

    return start_chunk;
  }

  Chunk_t* cur = start_chunk->next;
  Chunk_t* prev = start_chunk;
  while(cur != NULL)
  {
    if(cur->size > temp_chunk->size)
    {
      cur = cur->next;
      prev = prev->next;
    }
    else break;
  }

  if(cur)
  {
    prev->next = temp_chunk;
    temp_chunk->next = cur;

    return start_chunk;
  }
  else
  {
    prev->next = temp_chunk;
    return start_chunk;
  }
}

Chunk_t* create_chunk(size_t size)
{
  if(Heap->size < size)
  {
    /* Grow Heap */
  }
  Heap->size -= align(sizeof(Chunk_t)+size);
  Chunk_t* temp = Heap->allocated;
  Heap->allocated += align(sizeof(Chunk_t)+size);
  
  temp->Head = NULL;
  temp->next = NULL;
  temp->size = size;

  return temp;
}

Chunk_t* sort_chunk(Chunk_t* start_chunk, Chunk_t* temp_chunk)
{
  if(start_chunk == temp_chunk)
  {
    if(start_chunk->next == NULL) return start_chunk;
    start_chunk = temp_chunk->next;
    temp_chunk->next = NULL;
    return add_chunk(start_chunk,temp_chunk);
  }

  Chunk_t* cur = start_chunk;
  /* if cur == NULL error occurs need to be handle */
  while(cur->next != temp_chunk)
  {
    cur = cur->next;
  }

  cur->next = cur->next->next;
  temp_chunk->next = NULL;
  return add_chunk(start_chunk,temp_chunk);
}


