#ifndef FIND_BLOCK_H
#define FIND_BLOCK_H
#include "utils.h"

unsigned char find_array(size_t size);
void* find_array_chunk(unsigned char index);
unsigned char check_bitmap(Array_t* array_chunk);

void* find_block(size_t size);
Chunk_t* create_chunk(size_t size);
Chunk_t* add_chunk(Chunk_t* start_chunk,Chunk_t* temp_chunk);
Chunk_t* sort_chunk(Chunk_t* start_chunk, Chunk_t* temp_chunk);
Block_t* create_first_block(Chunk_t* temp, size_t size);
Block_t* best_fit_block(size_t size,Block_t* block);
Block_t* cut_block(Block_t* large_block, size_t size);

#endif