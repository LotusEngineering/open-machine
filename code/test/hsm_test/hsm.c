// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "hsm.h"

OM_ASSERT_FILE_NAME();

// Declare initial trans
static OmStateResult Hsm_init_trans(Hsm * const self);

// Declare all of the states and their parents
OM_STATE_DECLARE(Hsm, S1,   OM_TOP_STATE);
OM_STATE_DECLARE(Hsm, S11,  &S1);
OM_STATE_DECLARE(Hsm, S111, &S11);
OM_STATE_DECLARE(Hsm, S112, &S11);

OM_STATE_DECLARE(Hsm, S2, OM_TOP_STATE);
OM_STATE_DECLARE(Hsm, S21, &S2);

OM_STATE_DECLARE(Hsm, S3, OM_TOP_STATE);
OM_STATE_DECLARE(Hsm, S31, &S3);
OM_STATE_DECLARE(Hsm, S32, &S3);


void hsm_init(Hsm * const self, OmTrace* trace)
{
    OmTraceAttr trace_attr = {.name = "Hsm",
                              .trace = trace,
                              .flags = OM_TF_ALL};

    om_hsm_init(&self->base, 
                OM_INIT_CAST(Hsm_init_trans), 
                &trace_attr);

#if 0
    // Call base trace constructor with all tracing enabled
    om_hsm_init_trace(&self->base, 
            OM_INIT_CAST(Hsm_init_trans), 
            "Hsm", 
            trace,
            OM_TF_ALL);
#endif
}

OmStateResult Hsm_init_trans(Hsm * const self)
{
    return OM_TRANS(S1);
}

OM_STATE_DEFINE(Hsm, S1)
{
    OmStateResult result;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            result = OM_RES_HANDLED;
        break;
        case OM_EVT_INIT:
            result = OM_TRANS(S11);
        break;
        case OM_EVT_EXIT:
            result = OM_RES_HANDLED;
        break;
        case EVT_A:
            result = OM_TRANS(S2);
        break;
        case EVT_E:
            result = OM_RES_HANDLED;
        break;
        case EVT_I:
            result = OM_TRANS(S1);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(Hsm, S11)
{
    OmStateResult result;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            result = OM_RES_HANDLED;
        break;
        case OM_EVT_INIT:
            result = OM_TRANS(S111);
        break;
        case OM_EVT_EXIT:
            result = OM_RES_HANDLED;
        break;
        case EVT_D:
            result = OM_RES_HANDLED;
        break;
        case EVT_X:
            result = OM_EXIT(-1);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(Hsm, S111)
{
    OmStateResult result;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            result = OM_RES_HANDLED;
        break;
        case OM_EVT_EXIT:
            result = OM_RES_HANDLED;
        break;
        case EVT_C:
            result = OM_RES_HANDLED;
        break;
        case EVT_F:
            result = OM_TRANS(S112);
        break;
        case EVT_G:
            result = OM_TRANS(S21);
        break;
        case EVT_H:
            result = OM_TRANS(S111);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(Hsm, S112)
{
    OmStateResult result;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            result = OM_RES_HANDLED;
        break;
        case OM_EVT_EXIT:
            result = OM_RES_HANDLED;
        break;
        case EVT_F:
            result = OM_TRANS(S111);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}


OM_STATE_DEFINE(Hsm, S2)
{
    OmStateResult result;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            result = OM_RES_HANDLED;
        break;
        case OM_EVT_INIT:
            result = OM_TRANS(S21);
        break;
        case OM_EVT_EXIT:
            result = OM_RES_HANDLED;
        break;
        case EVT_B:
            result = OM_TRANS(S1);
        break;
        case EVT_X:
            result = OM_EXIT(-2);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(Hsm, S21)
{
    OmStateResult result;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            result = OM_RES_HANDLED;
        break;
        case OM_EVT_EXIT:
            result = OM_RES_HANDLED;
        break;
        case EVT_G:
            result = OM_TRANS(S112);
        break;
        case EVT_J:
            result = OM_TRANS(S31);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(Hsm, S3)
{
    OmStateResult result;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            result = OM_RES_HANDLED;
        break;
        case OM_EVT_EXIT:
            result = OM_RES_HANDLED;
        break;
        case EVT_J:
            result = OM_TRANS(S32);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(Hsm, S31)
{
    OmStateResult result;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            result = OM_RES_HANDLED;
        break;
        case OM_EVT_EXIT:
            result = OM_RES_HANDLED;
        break;
        case EVT_J:
            result = OM_TRANS(S3);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(Hsm, S32)
{
    OmStateResult result;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            result = OM_RES_HANDLED;
        break;
        case OM_EVT_EXIT:
            result = OM_RES_HANDLED;
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}
