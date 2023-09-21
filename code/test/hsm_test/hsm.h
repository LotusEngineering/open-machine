#ifndef HSM_H_
#define HSM_H_
#include "om.h"



enum HsmSignals
{
    EVT_A = OM_EVT_USER,
    EVT_B,
    EVT_C,
    EVT_D,
    EVT_E,
    EVT_F,
    EVT_G,
    EVT_H,
    EVT_I,
    EVT_J,
    EVT_X,
    EVT_Z
};

// Events used for testing HSM
OM_EVENT(EventA, EVT_A);
OM_EVENT(EventB, EVT_B);
OM_EVENT(EventC, EVT_C);
OM_EVENT(EventD, EVT_D);
OM_EVENT(EventE, EVT_E);
OM_EVENT(EventF, EVT_F);
OM_EVENT(EventG, EVT_G);
OM_EVENT(EventH, EVT_H);
OM_EVENT(EventI, EVT_I);
OM_EVENT(EventJ, EVT_J);
OM_EVENT(EventX, EVT_X);
OM_EVENT(EventZ, EVT_Z);


// The machine under test
typedef struct 
{
    OmMachine base;
}Hsm;

/**
 * @brief Constructs the machine under test
 * 
 * @param self 
 * @param trace 
 */
void hsm_ctor(Hsm * const self, OmTrace* trace);



#endif //MUIT_H_