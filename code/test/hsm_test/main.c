#include <stdio.h>
#include "unity.h"
#include "unity_fixture.h"
#include "mut_tests.c"

static void run_all_tests(void)
{
    RUN_TEST_GROUP(mut_tests);
}


int main(int argc, const char* argv[])
{
    return UnityMain(argc, argv, run_all_tests);
}

