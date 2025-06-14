#pragma obce

#include "../c/cprogramm.h"
#include "prepareint.h"

void CalculateStaticStack(CProgramm &p);
bool PrepareStaticArgumentsCall(Prepare &p, CNodePtr &node);
void PrepareFunction(Prepare &p);
bool PrepareStaticLoadVariable(Prepare &p, CNodePtr &node);
