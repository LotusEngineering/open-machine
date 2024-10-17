// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

/////////////// NOTE DO NOT COMPILE THIS FILE //////////////////////
// This is the start of a generic memory pool implementation that is not yet finished

#include <stdint.h>
#include "om_pool.h"
#include "om_assert.h"


OM_ASSERT_SET_FILE_NAME("om_pool.c");


typedef struct OmPoolBlock {
    uint8_t* data;
    struct OmPoolBlock* next;
} OmPoolBlock;

// See Note above if you get a redefinition error for this struct!
typedef struct OmPoolPort
{
//    uint8_t* block_storage;
//    OmPoolBlock* block_list;
    OmPoolBlock* free_list;
    int free_count;
//    OmMutex mutex;
}OmPoolPort;

static uint8_t small_block_storage[OM_POOL_SMALL_BLOCK_SIZE][OM_POOL_SMALL_BLOCK_COUNT];
static OmPoolBlock small_block_list[OM_POOL_SMALL_BLOCK_COUNT];

static uint8_t medium_block_storage[OM_POOL_MEDIUM_BLOCK_SIZE][OM_POOL_MEDIUM_BLOCK_COUNT];
static OmPoolBlock medium_block_list[OM_POOL_MEDIUM_BLOCK_COUNT];

static uint8_t large_block_storage[OM_POOL_LARGE_BLOCK_SIZE][OM_POOL_LARGE_BLOCK_COUNT];
static OmPoolBlock large_block_list[OM_POOL_LARGE_BLOCK_COUNT];

static OmPoolPort om_pool_table[OM_POOL_NUM_POOLS];


void om_pool_port_init_(OmPoolPort* port, uint8_t* block_storage, OmPoolBlock* block_list, int block_size, int number_of_blocks)
{
    for (int block_idx = 0; block_idx < number_of_blocks - 1; block_idx++)
    {
        // Set block data pointer
        block_list[block_idx].data = block_storage + (block_idx * block_size);
        block_list[block_idx].next = &block_list[block_idx + 1];
    }
    
    block_list[number_of_blocks - 1].next = NULL;

    port->free_count = number_of_blocks;
    port->free_list = &block_list[0];
    ///@TODO mutex

}

void om_pool_init(void)
{


    if (OM_POOL_NUM_POOLS >= 1)
    {
        om_pool_port_init_(&om_pool_table[0], small_block_storage, small_block_list, OM_POOL_SMALL_BLOCK_SIZE, OM_POOL_SMALL_BLOCK_COUNT);
    }
    else
    {
        // Must not call pool_init if you are allocating any pools
        OM_ERROR();
    }
    
    if (OM_POOL_NUM_POOLS >= 2)
    {
        om_pool_port_init_(&om_pool_table[1], medium_block_storage, medium_block_list, OM_POOL_MEDIUM_BLOCK_SIZE, OM_POOL_MEDIUM_BLOCK_COUNT);
    }

    if (OM_POOL_NUM_POOLS == 3)
    {
        om_pool_port_init_(&om_pool_table[2], large_block_storage, large_block_list, OM_POOL_LARGE_BLOCK_SIZE, OM_POOL_LARGE_BLOCK_COUNT);
    }

}


OmPoolEvent* om_pool_alloc(size_t event_size, OmSignal signal, const char* name)
{
    OmPoolEvent* new_event = NULL;
    int pool_index = -1;

    if(event_size <= OM_POOL_SMALL_BLOCK_SIZE)
    {
        pool_index = 0;
    }
    else if(event_size <= OM_POOL_MEDIUM_BLOCK_SIZE)
    {
        pool_index = 1;
    }
    else if(event_size <= OM_POOL_LARGE_BLOCK_SIZE)
    {
        pool_index = 2;
    }
    else
    {
        // Block size too big
        OM_ERROR();
    }

    // Ran out of free event space for this size
    OM_ASSERT(om_pool_table[pool_index].free_list != NULL);

    // Allocate block from head of free list
    OmPoolBlock* allocated_block = om_pool_table[pool_index].free_list;

    // Move head down the free list
    om_pool_table[pool_index].free_list = om_pool_table[pool_index].free_list->next;

    // Set base event data
    new_event = (OmPoolEvent*)allocated_block.data;
    new_event->base.type = OM_ET_POOL;
    new_event->base.signal = signal;
    new_event->base.name = name;
    new_event->reference_count = 0;
    new_event->port = &om_pool_table[pool_index];

    return new_event;
}



void om_pool_free(OmPoolEvent* event)
{
    // Cannot free null 
    OM_ASSERT (blockPtr != NULL);

    OmPoolBlockSmall* block = (OmPoolBlockSmall*)event;
    block->next = om_pool_small.free_list;
    om_pool_small.free_list = block;
}
