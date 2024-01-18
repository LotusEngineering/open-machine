// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "unity_fixture.h"

#include "om.h"
#include "hsm.h"

// Set assert file name
OM_ASSERT_FILE_NAME();

// Tracing suppoprt
#define TRACE_LIST_SIZE 48
OmTraceLogEntry trace_buffer[TRACE_LIST_SIZE];
OmTrace test_trace;

// Assert Support
static bool assert_hit = false;
void om_assert_handler(const char *file_name, int line)
{
    assert_hit = true;
    OM_TRACE_TWO(&test_trace, "ASSERT! File", file_name);
    OM_TRACE_VAL_DEC(&test_trace, line);
}

// The machine under test instance
Hsm hsm;

TEST_GROUP(hsm_tests);

TEST_SETUP(hsm_tests)
{
    assert_hit = false;

    // Construct Trace
    om_trace_init(&test_trace, trace_buffer, TRACE_LIST_SIZE);

    // Construct the test machine
    hsm_init(&hsm, &test_trace);
}

TEST_TEAR_DOWN(hsm_tests)
{
}

TEST(hsm_tests, trace_test)
{
    for (int i = 0; i < TRACE_LIST_SIZE - 1; i++)
    {
        TEST_ASSERT(om_trace_string(&test_trace, "Test"));
    }

    for (int i = 0; i < TRACE_LIST_SIZE - 1; i++)
    {
        OmTraceLogEntry trace;
        TEST_ASSERT(om_trace_read(&test_trace, &trace));

        TEST_ASSERT_EQUAL_STRING("Test", trace.message);
    }
}

TEST(hsm_tests, assert_test)
{
    OmTraceLogEntry trace;

    OM_ASSERT(false);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT(assert_hit);
}

TEST(hsm_tests, init_test)
{
    OmTraceLogEntry trace;

    // Dispatch Event A, dispatch should be ignored since machine not initialzed
    om_hsm_dispatch(&hsm.base, &EventA);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:om_hsm_dispatch:EVT_A:Ignored", trace.message);

    TEST_ASSERT_EQUAL(om_hsm_is_active(&hsm.base), false);

    // Enter the state machine
    om_hsm_enter(&hsm.base);

    TEST_ASSERT_EQUAL(om_hsm_is_active(&hsm.base), true);

    // Check traces
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:OM_TOP_STATE:om_hsm_enter():Trans:S1", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_ENTER:Handled", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_INIT:Trans:S11", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_ENTER:Handled", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_INIT:Trans:S111", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_ENTER:Handled", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_INIT:Ignored", trace.message);
}

TEST(hsm_tests, internal_trans_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_hsm_enter(&hsm.base);

    // Clear the log
    om_trace_clear(&test_trace);

    // Dispatch Event C, should be handled by S111 internally
    om_hsm_dispatch(&hsm.base, &EventC);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:EVT_C:Handled", trace.message);

    // Dispatch Event D, should be handled by S11 but ignored by S111
    om_hsm_dispatch(&hsm.base, &EventD);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:EVT_D:Ignored", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:EVT_D:Handled", trace.message);

    // Dispatch Event E, should be handled by S1 but ignored by S11 and S111
    om_hsm_dispatch(&hsm.base, &EventE);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:EVT_E:Ignored", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:EVT_E:Ignored", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:EVT_E:Handled", trace.message);

    // Dispatch Event Z, should be unhandled 
    om_hsm_dispatch(&hsm.base, &EventZ);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:EVT_Z:Ignored", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:EVT_Z:Ignored", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:EVT_Z:Ignored", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:OM_TOP_STATE:EVT_Z:Unhandled", trace.message);
}

TEST(hsm_tests, trans_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_hsm_enter(&hsm.base);

    // Clear the log
    om_trace_clear(&test_trace);

    // Dispatch Event A, should trans from S111 to S21
    om_hsm_dispatch(&hsm.base, &EventA);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:EVT_A:Ignored", trace.message);
    
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:EVT_A:Ignored", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:EVT_A:Trans:S2", trace.message);
    
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S2:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S2:OM_EVT_INIT:Trans:S21", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S21:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S21:OM_EVT_INIT:Ignored", trace.message);

    // Dispatch Event B, should trans from S21 to S1 and init into S111
    om_hsm_dispatch(&hsm.base, &EventB);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S21:EVT_B:Ignored", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S2:EVT_B:Trans:S1", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S21:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S2:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_INIT:Trans:S11", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_INIT:Trans:S111", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_INIT:Ignored", trace.message);
}

TEST(hsm_tests, side_trans_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_hsm_enter(&hsm.base);

    // Clear the log
    om_trace_clear(&test_trace);

    // Dispatch Event f, should trans from S111 to S112
    om_hsm_dispatch(&hsm.base, &EventF);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:EVT_F:Trans:S112", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S112:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S112:OM_EVT_INIT:Ignored", trace.message);

    // Dispatch Event f, should trans from S112 to S111
    om_hsm_dispatch(&hsm.base, &EventF);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S112:EVT_F:Trans:S111", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S112:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_INIT:Ignored", trace.message);

    // Dispatch Event G, should trans from S111 to S2
    om_hsm_dispatch(&hsm.base, &EventG);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:EVT_G:Trans:S21", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S2:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S21:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S21:OM_EVT_INIT:Ignored", trace.message);

    // Dispatch Event G, should trans from S2 to S112
    om_hsm_dispatch(&hsm.base, &EventG);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S21:EVT_G:Trans:S112", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S21:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S2:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S112:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S112:OM_EVT_INIT:Ignored", trace.message);
}

TEST(hsm_tests, self_trans_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_hsm_enter(&hsm.base);

    // Clear the log
    om_trace_clear(&test_trace);

    // Dispatch Event H, should trans from S111 back to S111
    om_hsm_dispatch(&hsm.base, &EventH);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:EVT_H:Trans:S111", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_INIT:Ignored", trace.message);

    // Dispatch Event I, should trans from S1 back to S1 
    om_hsm_dispatch(&hsm.base, &EventI);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:EVT_I:Ignored", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:EVT_I:Ignored", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:EVT_I:Trans:S1", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_ENTER:Handled", trace.message); 

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_INIT:Trans:S11", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_INIT:Trans:S111", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_INIT:Ignored", trace.message);

}

TEST(hsm_tests, local_trans_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_hsm_enter(&hsm.base);

    // Dispatch Event G, should trans to S21
    om_hsm_dispatch(&hsm.base, &EventG);

    // Clear the log
    om_trace_clear(&test_trace);

    // Dispatch Event J, should trans to S21
    om_hsm_dispatch(&hsm.base, &EventJ);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S21:EVT_J:Trans:S31", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S21:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S2:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S3:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S31:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S31:OM_EVT_INIT:Ignored", trace.message);

    // Send J again, should take us to S3 super state
    om_hsm_dispatch(&hsm.base, &EventJ);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S31:EVT_J:Trans:S3", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S31:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S3:OM_EVT_INIT:Ignored", trace.message);

    // Should not enter S3 again since we are already in it
    TEST_ASSERT_FALSE(om_trace_read(&test_trace, &trace));

    // Send J again, should take us to S32 sub state
    om_hsm_dispatch(&hsm.base, &EventJ);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S3:EVT_J:Trans:S32", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S32:OM_EVT_ENTER:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S32:OM_EVT_INIT:Ignored", trace.message);
}

TEST(hsm_tests, exit_with_signal_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_hsm_enter(&hsm.base);

    // Clear the log
    om_trace_clear(&test_trace);

    // Send X to exit machine
    om_hsm_dispatch(&hsm.base, &EventX);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:EVT_X:Ignored", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:EVT_X:Exit", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT_EQUAL(om_hsm_is_active(&hsm.base), false);

    // Make sure exit code was supplied
    TEST_ASSERT_EQUAL_INT(om_hsm_get_exit_code(&hsm.base), -1);

    // Dispatch Event A, dispatch should be ignored since machine has been exited
    om_hsm_dispatch(&hsm.base, &EventA);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:om_hsm_dispatch:EVT_A:Ignored", trace.message);

}

TEST(hsm_tests, exit_with_function_call_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_hsm_enter(&hsm.base);

    // Clear the log
    om_trace_clear(&test_trace);

    // Call Exit function with code -2
    om_hsm_exit(&hsm.base, -2);

    
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:om_hsm_exit()", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S111:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S11:OM_EVT_EXIT:Handled", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:S1:OM_EVT_EXIT:Handled", trace.message);

    // Should be at top of machine
    TEST_ASSERT(om_trace_read(&test_trace, &trace) == false);

    TEST_ASSERT_EQUAL(om_hsm_is_active(&hsm.base), false);

    // Make sure exit code was supplied
    TEST_ASSERT_EQUAL_INT(om_hsm_get_exit_code(&hsm.base), -2);

    // Dispatch Event A, dispatch should be ignored since machine has been exited
    om_hsm_dispatch(&hsm.base, &EventA);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Hsm:om_hsm_dispatch:EVT_A:Ignored", trace.message);

}

TEST_GROUP_RUNNER(hsm_tests)
{
    RUN_TEST_CASE(hsm_tests, trace_test);
    RUN_TEST_CASE(hsm_tests, assert_test);
    RUN_TEST_CASE(hsm_tests, init_test);
    RUN_TEST_CASE(hsm_tests, internal_trans_test);
    RUN_TEST_CASE(hsm_tests, trans_test);
    RUN_TEST_CASE(hsm_tests, side_trans_test);
    RUN_TEST_CASE(hsm_tests, self_trans_test);
    RUN_TEST_CASE(hsm_tests, local_trans_test);
    RUN_TEST_CASE(hsm_tests, exit_with_signal_test);
    RUN_TEST_CASE(hsm_tests, exit_with_function_call_test);
}
