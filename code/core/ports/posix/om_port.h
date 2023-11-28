// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_PORT_H_
#define OM_PORT_H_

#include <pthread.h> 
#include "om_queue.h"
#include "om_event.h"

typedef pthread_mutex_t OmMutexPort;

/**
 * @brief Actor Port definition for POSIX
 * 
 */
typedef struct OmActorPort
{
    pthread_t thread_id;
    OmQueue queue;
    OmEvent** queue_storage;
    pthread_mutex_t q_mutex;
    pthread_cond_t q_cond;
}OmActorPort;


// We are just using malloc for now
typedef struct OmPoolPort
{

}OmPoolPort;

#endif //OM_PORT_H_