#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <stdio.h>
#include "rbtree.h"

#define word_t intptr_t     // Mahcine Word  64-8bytes 32-4bytes
#define THRESHOLD_ARRAY 128
#define HEAP_MIN 131072     // 128k
#define MINI 8192
#define SMALL 32768
#define MEDIUM 65536
#define LARGE 131072

#define align(n) (n + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1)
#define allocSizeHeap(size) (sizeof(Heap_t) + (size))
#define allocSizeChunk(size) (sizeof(Chunk_t) + (size))
#define get_addr(addr) (addr & ~(0xf))
#define get_size(addr) (addr & (0xf))

typedef struct Array
{
  size_t bitmap;
  word_t data[1];
} Array_t;

typedef struct Block
{
  struct Block* parent;
  struct Block* left;
  struct Block* right;
  size_t size;
  size_t prev_size;
  word_t data[1];
} Block_t;

typedef struct Chunk        // Use chunk to avoid multiple system call
{         
  struct Chunk* next;      
  size_t size;              // Here should store the max size of free block attached to the chunk
  Block_t* Head;
} Chunk_t;

typedef struct Heap
{
  size_t size;
  void* allocated;
  Array_t* array_Chunk[16];
  Chunk_t* linked_Chunk[4];        // 8k 32k 64k 128k
}Heap_t;


extern Heap_t* Heap;

#endif