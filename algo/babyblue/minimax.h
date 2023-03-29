#ifndef MINIMAX_H
#define MINIMAX_H

#include "heuristic.h"
#include "main.h"
#include "map.h"
#include "ship.h"

/* STATE.OUT
totalTurn
enemygold enemyshield
[[oldmap]]
*/

class MinimaxAgent {
private:
  int n, m, k, totalTurn, lastMove;
  Ship myShip, enemyShip;
  Map sea, oldsea;

  // TODO: Avoid go back old posittion
  //
  pair<int, int> MaxNode(int alpha, int beta, int turn, int depth);
  pair<int, int> MinNode(int alpha, int beta, int turn, int depth);

public:
  MinimaxAgent();

  void Input(ifstream &inp);
  void Load(ifstream &sav);
  void Save(ofstream &sav);
  void MakeMove(ofstream &out);
};

#endif  // MINIMAX_H
