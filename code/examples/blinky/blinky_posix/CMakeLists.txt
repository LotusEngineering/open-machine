cmake_minimum_required(VERSION 3.22.1)

project(can_flash C)



add_executable(${PROJECT_NAME} "")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR} 
    ../../../core/inc
    ../../../ports/posix
    ../
    )

target_sources(${PROJECT_NAME} PRIVATE 
            ../../../core/src/om_machine.c
            ../../../core/src/om_trace.c
            ../../../core/src/om_timer.c
            ../../../core/src/om_queue.c
            ../../../core/ports/posix/om_actor_posix.c
            ../../../core/ports/posix/om_pool_posix.c
            ../blinky.c
            ../application.c
            board.c
            main.c
            )
