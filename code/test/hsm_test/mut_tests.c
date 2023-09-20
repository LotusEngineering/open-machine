#include "unity_fixture.h"

#include "om.h"
#include "mut.h"

// Set assert file name
OM_ASSERT_SET_FILE_NAME();

// Tracing suppoprt
#define TRACE_LIST_SIZE 48
OmTraceLogEntry trace_buffer[TRACE_LIST_SIZE];
OmTrace test_trace;

// Assert Support
static bool assert_hit = false;
void om_assert_handler(const char *file_name, int line)
{
    assert_hit = true;
    om_trace_write(&test_trace, "ASSERT! File: %s, Line: %d", file_name, line);
}

// The machine under test instance
Mut mut;

TEST_GROUP(mut_tests);

TEST_SETUP(mut_tests)
{
    assert_hit = false;

    // Construct Trace
    om_trace_ctor(&test_trace, trace_buffer, TRACE_LIST_SIZE);

    // Construct the test machine
    mut_ctor(&mut, &test_trace);
}

TEST_TEAR_DOWN(mut_tests)
{
}

TEST(mut_tests, trace_test)
{
    for (int i = 0; i < TRACE_LIST_SIZE - 1; i++)
    {
        TEST_ASSERT(om_trace_write(&test_trace, "%d", i));
    }

    for (int i = 0; i < TRACE_LIST_SIZE - 1; i++)
    {
        OmTraceLogEntry trace;
        TEST_ASSERT(om_trace_read(&test_trace, &trace));

        char buffer[8];
        itoa(i, buffer, 10);
        TEST_ASSERT_EQUAL_STRING(buffer, trace.message);
    }
}

TEST(mut_tests, assert_test)
{
    OmTraceLogEntry trace;

    OM_ASSERT(false);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT(assert_hit);
}

TEST(mut_tests, init_test)
{
    OmTraceLogEntry trace;

    // Dispatch Event A, dispatch should be ignored since machine not initialzed
    om_dispatch(&mut.base, &EventA);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:om_dispatch:EVT_A:IGNORED", trace.message);

    // Initialize the state machine
    om_enter(&mut.base);

    // Check traces
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:OM_TOP_STATE:om_enter():TRANS(S1)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:OM_EVT_ENTER:HANDLED", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:OM_EVT_INIT:TRANS(S11)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:OM_EVT_ENTER:HANDLED", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:OM_EVT_INIT:TRANS(S111)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_ENTER:HANDLED", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_INIT:IGNORED", trace.message);
}

TEST(mut_tests, internal_trans_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_enter(&mut.base);

    // Clear the log
    om_trace_clear(&test_trace);

    // Dispatch Event C, should be handled by S111 internally
    om_dispatch(&mut.base, &EventC);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:EVT_C:HANDLED", trace.message);

    // Dispatch Event D, should be handled by S11 but ignored by S111
    om_dispatch(&mut.base, &EventD);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:EVT_D:IGNORED", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:EVT_D:HANDLED", trace.message);

    // Dispatch Event E, should be handled by S1 but ignored by S11 and S111
    om_dispatch(&mut.base, &EventE);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:EVT_E:IGNORED", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:EVT_E:IGNORED", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:EVT_E:HANDLED", trace.message);

    // Dispatch Event Z, should be unhandled 
    om_dispatch(&mut.base, &EventZ);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:EVT_Z:IGNORED", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:EVT_Z:IGNORED", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:EVT_Z:IGNORED", trace.message);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:OM_TOP_STATE:EVT_Z:Unhandled", trace.message);
}

TEST(mut_tests, trans_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_enter(&mut.base);

    // Clear the log
    om_trace_clear(&test_trace);

    // Dispatch Event A, should trans from S111 to S21
    om_dispatch(&mut.base, &EventA);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:EVT_A:IGNORED", trace.message);
    
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:EVT_A:IGNORED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:EVT_A:TRANS(S2)", trace.message);
    
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S2:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S2:OM_EVT_INIT:TRANS(S21)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S21:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S21:OM_EVT_INIT:IGNORED", trace.message);

    // Dispatch Event B, should trans from S21 to S1 and init into S111
    om_dispatch(&mut.base, &EventB);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S21:EVT_B:IGNORED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S2:EVT_B:TRANS(S1)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S21:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S2:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:OM_EVT_INIT:TRANS(S11)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:OM_EVT_INIT:TRANS(S111)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_INIT:IGNORED", trace.message);
}

TEST(mut_tests, side_trans_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_enter(&mut.base);

    // Clear the log
    om_trace_clear(&test_trace);

    // Dispatch Event f, should trans from S111 to S112
    om_dispatch(&mut.base, &EventF);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:EVT_F:TRANS(S112)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S112:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S112:OM_EVT_INIT:IGNORED", trace.message);

    // Dispatch Event f, should trans from S112 to S111
    om_dispatch(&mut.base, &EventF);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S112:EVT_F:TRANS(S111)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S112:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_INIT:IGNORED", trace.message);

    // Dispatch Event G, should trans from S111 to S2
    om_dispatch(&mut.base, &EventG);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:EVT_G:TRANS(S21)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S2:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S21:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S21:OM_EVT_INIT:IGNORED", trace.message);

    // Dispatch Event G, should trans from S2 to S112
    om_dispatch(&mut.base, &EventG);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S21:EVT_G:TRANS(S112)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S21:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S2:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S112:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S112:OM_EVT_INIT:IGNORED", trace.message);
}

TEST(mut_tests, self_trans_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_enter(&mut.base);

    // Clear the log
    om_trace_clear(&test_trace);

    // Dispatch Event H, should trans from S111 back to S111
    om_dispatch(&mut.base, &EventH);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:EVT_H:TRANS(S111)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_INIT:IGNORED", trace.message);

    // Dispatch Event I, should trans from S1 back to S1 
    om_dispatch(&mut.base, &EventI);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:EVT_I:IGNORED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:EVT_I:IGNORED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:EVT_I:TRANS(S1)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:OM_EVT_EXIT:HANDLED", trace.message);/// Problem, not exiting S1

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:OM_EVT_ENTER:HANDLED", trace.message); 

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:OM_EVT_INIT:TRANS(S11)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:OM_EVT_INIT:TRANS(S111)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_INIT:IGNORED", trace.message);

}

TEST(mut_tests, local_trans_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_enter(&mut.base);

    // Dispatch Event G, should trans to S21
    om_dispatch(&mut.base, &EventG);

    // Clear the log
    om_trace_clear(&test_trace);

    // Dispatch Event J, should trans to S21
    om_dispatch(&mut.base, &EventJ);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S21:EVT_J:TRANS(S31)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S21:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S2:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S3:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S31:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S31:OM_EVT_INIT:IGNORED", trace.message);

    // Send J again, should take us to S3 super state
    om_dispatch(&mut.base, &EventJ);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S31:EVT_J:TRANS(S3)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S31:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S3:OM_EVT_INIT:IGNORED", trace.message);

    // Should not enter S3 again since we are already in it
    TEST_ASSERT_FALSE(om_trace_read(&test_trace, &trace));

    // Send J again, should take us to S32 sub state
    om_dispatch(&mut.base, &EventJ);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S3:EVT_J:TRANS(S32)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S32:OM_EVT_ENTER:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S32:OM_EVT_INIT:IGNORED", trace.message);
}

TEST(mut_tests, exit_test)
{
    OmTraceLogEntry trace;

    // Initialize the state machine
    om_enter(&mut.base);

    // Clear the log
    om_trace_clear(&test_trace);

    // Send X to exit machine
    om_dispatch(&mut.base, &EventX);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:EVT_X:IGNORED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:EVT_X:EXIT(-1)", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S111:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S11:OM_EVT_EXIT:HANDLED", trace.message);

    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:S1:OM_EVT_EXIT:HANDLED", trace.message);

    // Make sure exit code was supplied
    TEST_ASSERT_EQUAL_INT(om_get_exit_code(&mut.base), -1);

    // Dispatch Event A, dispatch should be ignored since machine has been exited
    om_dispatch(&mut.base, &EventA);
    TEST_ASSERT(om_trace_read(&test_trace, &trace));
    TEST_ASSERT_EQUAL_STRING("Mut:om_dispatch:EVT_A:IGNORED", trace.message);

}

TEST_GROUP_RUNNER(mut_tests)
{
    RUN_TEST_CASE(mut_tests, trace_test);
    RUN_TEST_CASE(mut_tests, assert_test);
    RUN_TEST_CASE(mut_tests, init_test);
    RUN_TEST_CASE(mut_tests, internal_trans_test);
    RUN_TEST_CASE(mut_tests, trans_test);
    RUN_TEST_CASE(mut_tests, side_trans_test);
    RUN_TEST_CASE(mut_tests, self_trans_test);
    RUN_TEST_CASE(mut_tests, local_trans_test);
    RUN_TEST_CASE(mut_tests, exit_test);
}
