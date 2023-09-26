// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <stdio.h>
#include "unity.h"
#include "unity_fixture.h"
#include "hsm_tests.c"

static void run_all_tests(void)
{
    RUN_TEST_GROUP(hsm_tests);
}


int main(int argc, const char* argv[])
{
    return UnityMain(argc, argv, run_all_tests);
}

