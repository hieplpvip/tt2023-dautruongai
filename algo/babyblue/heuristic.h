#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "main.h"
#include "map.h"
#include "random.h"
#include "ship.h"

class Heuristic {
public:
  // (1) Find position where the ship can be placed to maximize the number of point in the range of RANGE
  // (2) Select 3 candidate positions and choose random to avoid conciding enemy's move
  static pair<int, int> FirstPlace(Map &sea, Ship &myShip, Ship &enemyShip);

  // TODO: Evaluate treasure
  // TODO: set range = min(RANGE, TURN_LEFT)
  // (1) Select KSEED posittions having the highest gold in the range RANGE
  // (2) Score is the difference between myShipScore and enemyShipScore
  // (3) If myShip drown (two ship collide or meet barrier), return -INF
  static int Evaluate(Map &sea, const Ship &myShip, const Ship &enemyShip);
};

#endif  // HEURISTIC_H
