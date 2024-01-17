// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "om_pool.h"
#include "om_config.h"
#include "om_assert.h"


OM_ASSERT_FILE_NAME();



void om_pool_init(void)
{
    // Lite port doesn't have memory pools
    OM_ASSERT(false);
}


OmPoolEvent* om_pool_alloc(size_t event_size, OmSignal signal, const char* name)
{
    (void)event_size;
    (void)signal;
    (void)name;

    // Lite port doesn't have memory pools
    OM_ASSERT(false);

    return NULL;
}



void om_pool_free(OmPoolEvent* event)
{
    (void)event;
    
    // Lite port doesn't have memory pools
    OM_ASSERT(false);
}
