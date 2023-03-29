#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "main.h"
#include "map.h"
#include "random.h"
#include "ship.h"

class Heuristic {
private:
  // Get the total (gold - dist) in the range (choose 'lim' highest values)
  // Ignoring gold in the enemy zone (dist(enemyShip, pos) <= EZONE)
  static int bfs(Map &sea, int u, int v, int _u, int _v, int s, int lim, int range);

  // Return value of the first (gold) reached
  // Ignore gold if enemy can come first
  static int bfs2(Map &sea, int u, int v, int _u, int _v, int s, int lim);

public:
  // (1) Find position where the ship can be placed to maximize the number of point in the range of RANGE
  // (2) Select 3 candidate positions and choose random to avoid conciding enemy's move
  static pair<int, int> FirstPlace(Map &sea);

  // TODO: Evaluate treasure
  // TODO: set range = min(RANGE, TURN_LEFT)
  // (1) Select KSEED posittions having the highest gold in the range RANGE
  // (2) Score is the difference between myShipScore and enemyShipScore
  // (3) If myShip drown (two ship collide or meet barrier), return -INF
  static int Evaluate(Map &sea, const Ship &myShip, const Ship &enemyShip);
};

#endif  // HEURISTIC_H
