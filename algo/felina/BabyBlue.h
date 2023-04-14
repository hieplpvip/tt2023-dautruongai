#ifndef BABYBLUE_H
#define BABYBLUE_H

#include "State.h"
#include "Store.h"
#include <bits/stdc++.h>

using namespace std;

namespace BabyBlue {
  constexpr int MAXN = 20;
  constexpr int INF = 100'000'000;

  constexpr int MAX_DEPTH = 10;
  constexpr int KSEED = 15;
  constexpr int DANGER = -1000;
  constexpr int SHIELD = 100;
  constexpr int TREASURE = 100;
  constexpr int KRAND = 2;
  constexpr int EZONE = 3;
  constexpr int BONUS = 10;
  constexpr int RATE = 5;

  class Ship {
  public:
    void Move(int dir);
    void Place(int x, int y);

    int x, y, g, s;
    score_t score;
  };

  class Map {
  public:
    void Read(int _n, int _m);
    bool isValid(int x, int y, int s);

    int n, m;
    std::string at[20][20];
    int val[20][20];
  };

  class Heuristic {
  public:
    static std::tuple<score_t, int, int> BestPlace(Map &sea, Ship &myShip, Ship &enemyShip, int range, int k, int totalTurn, bool first = true);
    static std::tuple<score_t, int, int> HighestHeat(Map &sea, Ship &myShip, Ship &enemyShip, int range, int k, int totalTurn, bool ignoreShield);
  };

  class MinimaxAgent {
  private:
    int n, m, k, totalTurn, lastMove;
    Ship myShip, enemyShip;
    Map sea;
    int _mx[MAX_DEPTH + 5], _my[MAX_DEPTH + 5], _mg[MAX_DEPTH + 5], _ms[MAX_DEPTH + 5];
    int _ex[MAX_DEPTH + 5], _ey[MAX_DEPTH + 5], _eg[MAX_DEPTH + 5], _es[MAX_DEPTH + 5];

    std::pair<score_t, int> MaxNode(score_t alpha, score_t beta, int turn, int depth);
    std::pair<score_t, int> MinNode(score_t alpha, score_t beta, int turn, int depth);

  public:
    void Input();
    void Load();
    void MakeMove();
  };

  void run();
}

#endif
