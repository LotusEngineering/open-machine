cmake_minimum_required(VERSION 3.27.4)

project(hsm_test C)



add_executable(${PROJECT_NAME} "")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR} 
    ../../core/inc
    ../unity/src 
    ../unity/extras/fixture/src
    ../unity/extras/memory/src)

target_sources(${PROJECT_NAME} PRIVATE 
            ../../core/src/om_machine.C
            ../../core/src/om_trace.c
            ../unity/src/unity.c
            ../unity/extras/fixture/src/unity_fixture.c
            hsm.c
            main.c
            )
