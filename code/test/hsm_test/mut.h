#ifndef MUT_H_
#define MUT_H_
#include "om.h"


enum MutSignals
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

// Events used for testing
static const OmEvent EventA = {EVT_A, "EVT_A"};
static const OmEvent EventB = {EVT_B, "EVT_B"};
static const OmEvent EventC = {EVT_C, "EVT_C"};
static const OmEvent EventD = {EVT_D, "EVT_D"};
static const OmEvent EventE = {EVT_E, "EVT_E"};
static const OmEvent EventF = {EVT_F, "EVT_F"};
static const OmEvent EventG = {EVT_G, "EVT_G"};
static const OmEvent EventH = {EVT_H, "EVT_H"};
static const OmEvent EventI = {EVT_I, "EVT_I"};
static const OmEvent EventJ = {EVT_J, "EVT_J"};
static const OmEvent EventX = {EVT_X, "EVT_X"};
static const OmEvent EventZ = {EVT_Z, "EVT_Z"};


// The machine under test
typedef struct 
{
    OmMachine base;
}Mut;

/**
 * @brief Constructs the machine under test
 * 
 * @param self 
 * @param trace 
 */
void mut_ctor(Mut * const self, OmTrace* trace);



#endif //MUIT_H_