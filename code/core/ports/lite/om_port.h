// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_PORT_H_
#define OM_PORT_H_

/// @brief Lite port doesn't have mutexes
typedef void* OmMutexPort;

/**
 * @brief Actor Port for lite
 * 
 */
typedef struct OmActorPort
{
    void* lite;
}OmActorPort;


/// @brief Lite port doesn't have a memory pool
typedef struct OmPoolPort
{
    void* lite;
}OmPoolPort;

#endif //OM_PORT_H_