#ifndef MINIMAX_H
#define MINIMAX_H

#include "heuristic.h"
#include "main.h"
#include "map.h"
#include "ship.h"

class MinimaxAgent {
private:
  int k;
  Ship myShip, enemyShip;
  Map sea;

  pair<int, int> MaxNode(int alpha, int beta, int depth = 0);
  pair<int, int> MinNode(int alpha, int beta, int depth = 0);

public:
  MinimaxAgent();

  void Input(ifstream &inp);
  void MakeMove(ofstream &out);
};

#endif  // MINIMAX_H
