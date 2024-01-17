
#include <stddef.h>
#include "om_queue.h"
#include "om_assert.h"

OM_ASSERT_SET_FILE_NAME("om_queue.c");

void om_queue_init(OmQueue* self, OmEvent ** const queue_storage,  size_t queue_size)
{
    self->store = queue_storage;
    self->store_size = queue_size;
    self->read_index = 0;
    self->write_index = 0;
}

bool om_queue_put(OmQueue* self, OmEvent const *const event)
{
    size_t next_write_index = self->write_index + 1;

    if (next_write_index >= self->store_size)
    {
        next_write_index = 0;
    }

    // Check for full
    if (next_write_index == self->read_index)
    {
        return false;
    }
    else
    {
        // Copy into storage
        self->store[self->write_index] = event;

        // Update next write index
        self->write_index = next_write_index;

        return true;
    }    
}

bool om_queue_get(OmQueue* self, OmEvent const ** event)
{
    if (self->write_index == self->read_index)
    {
        return false;
    }

    size_t next_read_index = self->read_index + 1;  
    if(next_read_index >= self->store_size)
    {
        next_read_index = 0;
    }

    // Read event
    *event = self->store[self->read_index];

    self->read_index = next_read_index;

    return true;
}
