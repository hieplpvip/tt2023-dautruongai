#ifndef ENGINE_H
#define ENGINE_H

#include "State.h"
#include "Utility.h"
#include <vector>

extern State rootState;
extern std::vector<Position> shieldPos;

void initializeStore();
void handleFirstTurn();
void handleOtherTurns();

#endif
