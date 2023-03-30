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
  int _mx[MAX_DEPTH + 5], _my[MAX_DEPTH + 5], _mg[MAX_DEPTH + 5];
  int _ex[MAX_DEPTH + 5], _ey[MAX_DEPTH + 5], _eg[MAX_DEPTH + 5];

  // TODO: Avoid go back old posittion
  //
  pair<score_t, int> MaxNode(score_t alpha, score_t beta, int turn, int depth);
  pair<score_t, int> MinNode(score_t alpha, score_t beta, int turn, int depth);

public:
  MinimaxAgent();

  void Input(ifstream &inp);
  void Load(ifstream &sav);
  void Save(ofstream &sav);
  void MakeMove(ofstream &out);
};

#endif  // MINIMAX_H
