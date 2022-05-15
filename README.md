Reopen it with a codeviewer

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
