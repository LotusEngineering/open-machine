#include "mut.h"

OM_ASSERT_SET_FILE_NAME();

// Declare initial trans
static OmStateResult Mut_init_trans(Mut * const self);

// Declare all of the states and their parents
OM_STATE_DECLARE(S1, Mut, OM_TOP_STATE);
OM_STATE_DECLARE(S11, Mut, &S1);
OM_STATE_DECLARE(S111, Mut, &S11);
OM_STATE_DECLARE(S112, Mut, &S11);

OM_STATE_DECLARE(S2, Mut, OM_TOP_STATE);
OM_STATE_DECLARE(S21, Mut, &S2);

OM_STATE_DECLARE(S3, Mut, OM_TOP_STATE);
OM_STATE_DECLARE(S31, Mut, &S3);
OM_STATE_DECLARE(S32, Mut, &S3);


void mut_ctor(Mut * const self, OmTrace* trace)
{
    // Call base constructor
    om_ctor(&self->base, 
            OM_INIT_CAST(Mut_init_trans), 
            "Mut", 
            trace);
}

OmStateResult Mut_init_trans(Mut * const self)
{
    return OM_TRANS(S1);
}

OM_STATE_DEFINE(S1, Mut)
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

OM_STATE_DEFINE(S11, Mut)
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

OM_STATE_DEFINE(S111, Mut)
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

OM_STATE_DEFINE(S112, Mut)
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


OM_STATE_DEFINE(S2, Mut)
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

OM_STATE_DEFINE(S21, Mut)
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

OM_STATE_DEFINE(S3, Mut)
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

OM_STATE_DEFINE(S31, Mut)
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

OM_STATE_DEFINE(S32, Mut)
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
