#ifndef USER_INTERFACE_H_
#define USER_INTERFACE_H_
#include "om.h"

typedef struct 
{
    OmActor base;
}UserInterface;

void ui_ctor(UserInterface* self, OmTrace* trace);

#endif //USER_INTERFACE_H_ 
