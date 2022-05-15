/*
MALLOCATOR


A Global HEAP
typedef struct Heap
{
  size_t size;
  void* allocated;
  Array_t* array_Chunk[16];
  Chunk_t* linked_Chunk[4];        // 8k 32k 64k 128k
}Heap_t;


array_Chunk use bit to achieve small memory alloc which vary from 8byte to 128byte
typedef struct Array
{
  size_t bitmap;
  word_t data[1];
} Array_t;

addr formats:
------------------------------------
|Addr  |bitmap|   1  |   2  |   3  |
------------------------------------


there are 4 kinds of linked_Chunk
8KB
32KB
64KB
128KB

typedef struct Chunk       
{         
  struct Chunk* next;      
  size_t size;              
  Block_t* Head;
} Chunk_t;

typedef struct Block
{
  struct Block* parent;
  struct Block* left;
  struct Block* right;
  size_t size;
  size_t prev_size;
  word_t data[1];
} Block_t;


|--------|           |--------|
| nAddr  | --------> | nAddr  |
|--------|           |--------|
|  left  |           |  left  |
|  size  |           |  size  |
|--------|           |--------|
|  data  |           |  data  |
|  begin |           |  begin |
|--------|           |--------|
     |
     |
     |
     |
     |
     |
     |
     ---->    |--------------|
              |    parent    |
              |--------------|
              |     left     |
              |--------------|
              |    right     |
              |--------------|
              |     size     |
              |--------------|
              |   prev_size  |
              |--------------|
              |  data_begin  |
              |--------------| 

In my machine the machine word is 16byte
the size grow in 16x mode
cur addr is            0xfffffff70
next addr should be    0xfffffff80
The last 4 bit remained unused

Last 4 bit will be like 
-----------------------------------------
|    0    |    0    |    1    |    0    |
-----------------------------------------

The first 2 bit represent which block it blong
The third bit indicates whether it is currently in use. During garbage collection, the Header of the previous block can be obtained by subtracting the size of the previous block from the current address, and then the use bit in the Header of the previous block tells whether the block is using . If not, the two Block can be combine.
The fourth bit represent the color of the RBtree, 0 for red, 1 for black.


Chunk is a linked list, we first find a suitable chunk and in the chunk we create a block or use the current unused block during the alloc.



alloc->alloc_T(use array or chunk)->invoke the function->return the addr
free->compare the addr with array addr(inrange:reset bitmap outrange:keep going)->(Get Header)->check if need combine->put it back into the chunk
*/




#include "allocator.h"
#include "gc.h"
#include <time.h>
#include <stdlib.h>

void array_test0();
void test0();
void test1();
void test2();

int main()
{
  /* write some testcase here */
  printf("----test start----\n");
  test0();
  return 0;
}

void array_test0()
{
  int *a = NULL;
  for(int i=0;i<64;i++)
  {
    a = (int*)alloc(sizeof(int));
    mfree(a);
  }
  printf("a         :%p\n",a);
  printf("allocated :%p\n",Heap->allocated);
  printf("sizeof :%016lx\n",Heap->array_Chunk[0]->bitmap);
  
}

void test0()
{
  int *a = (int*)alloc(sizeof(int));
  printf("a: %p\n",a);
  int *b = (int*)alloc(sizeof(int));
  printf("b: %d\n");
}

void test1()
{
  int *a = (int*) alloc(1024*sizeof(int));
  printf("a: %p\n",a);
  int *b = (int*) alloc(1023*sizeof(int));
  printf("b: %p\n",b);
  int *c = (int*) alloc(512*sizeof(int));
  printf("c: %p\n",c);
  a[1023] = 10000;
  c[0] = 23;
  printf("a[1023] : %d\n",a[1023]);
  printf("c[0]    : %d\n",c[0]);
}

void test2()
{
  /* alloc & free test */
  void *p = sbrk(0);
  int *a = (int*)alloc(sizeof(int)*10);

  int *b = (int*)alloc(sizeof(int)*5);
  int *c = (int*)alloc(sizeof(int)*6);
  int *d = (int*)alloc(sizeof(int)*7);
  
  
  a[5] = 10;
  free(a);
  free(b);


  free(c);
  free(d);
}

void test3()
{
  /* time test */
  /* alloc 5000 times */
  int i = 0;
  int j = 0;
  double time1,time2;
  clock_t startclock1, endclock1, startclock2, endclock2;

  startclock1 = clock();
  for(i=0;i<500000;i++)
  {
    int i = rand() %1000;
    sbrk(i);
  }
  endclock1 = clock();
  startclock2 = clock();
  for(j=0;j<500000;j++)
  {
    int i = rand()%1000;
    malloc(i);
  }
  endclock2 = clock();

  time1 = (double) (endclock1-startclock1)/CLOCKS_PER_SEC;
  time2 = (double) (endclock2-startclock2)/CLOCKS_PER_SEC;
  printf("sbrk:     %f\n", time1);
  printf("malloc:   %f\n", time2);

}
