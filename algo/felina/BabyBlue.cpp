#include "BabyBlue.h"
#include "Random.h"
#include <algorithm>
#include <fstream>
#include <queue>
#include <utility>
#include <vector>

using namespace std;

namespace BabyBlue {
#define sqr(a) ((a) * (a))
#define dist(a, b, c, d) (abs((a) - (c)) + abs((b) - (d)))
#define bval(a, b, c) max(a, min(b, c))
#define f(gold, dist) (sqr(gold) / sqrt(dist))

  void Ship::Move(int dir) {
    x += dx[dir];
    y += dy[dir];
  }

  void Ship::Place(int _x, int _y) {
    this->x = _x;
    this->y = _y;
  }

  void Map::Read(int _n, int _m) {
    this->n = _n;
    this->m = _m;
    for (int i = 1; i <= n; ++i) {
      for (int j = 1; j <= m; ++j) {
        int cell = rootState.at[i - 1][j - 1];
        if (cell == DANGER_CELL) {
          at[i][j] = "D";
        } else if (cell == SHIELD_CELL) {
          at[i][j] = "S";
        } else {
          at[i][j] = to_string(cell);
        }

        if (at[i][j] == "D") {
          val[i][j] = DANGER;
        } else if (at[i][j] == "S") {
          val[i][j] = SHIELD;
        } else {
          val[i][j] = stoi(at[i][j]);
        }
      }
    }
  }

  bool Map::isValid(int x, int y, int s) {
    return x >= 1 && x <= n && y >= 1 && y <= m && (s || at[x][y] != "D");
  }

  tuple<score_t, int, int> Heuristic::BestPlace(Map &sea, Ship &myShip, Ship &enemyShip, int range, int k, int totalTurn, bool first) {
    vector<tuple<score_t, int, int>> cand;

    int n = sea.n, m = sea.m;
    for (int i = 1; i <= n; ++i) {
      for (int j = 1; j <= m; ++j) {
        if (first && sea.at[i][j] != "0") {
          continue;
        }

        if (first) {
          myShip.Place(i, j);
          enemyShip.Place(-INF, -INF);
        }
        auto [gold, x, y] = HighestHeat(sea, myShip, enemyShip, range, k, totalTurn, false);
        cand.emplace_back(gold, i, j);
      }
    }

    sort(cand.rbegin(), cand.rend());

    int krand = KRAND;
    while (krand > 1 && get<0>(cand[krand - 1]) <= -INF + 5) krand--;
    return cand[Random::rand(krand)];
  }

  tuple<score_t, int, int> Heuristic::HighestHeat(Map &sea, Ship &myShip, Ship &enemyShip, int range, int k, int totalTurn, bool ignoreShield) {
    int n = sea.n, m = sea.m;
    int u = myShip.x, v = myShip.y, s = myShip.s;
    int _u = enemyShip.x, _v = enemyShip.y;
    score_t val;

    if (ignoreShield) {
      s = 1;
    }

    vector<vector<score_t>> table(n + 5, vector<score_t>(m + 5, 0));

    if (k > totalTurn / 2) {
      sea.val[n / 2 + 1][m / 2 + 1] = sqr(abs(myShip.g - enemyShip.g) * 3 / 4) / (k - totalTurn / 2 + 2);
    }

    queue<pair<int, int>> Q;
    vector<vector<int>> visited(n + 1, vector<int>(m + 1, -1));

    Q.emplace(u, v);
    visited[u][v] = 0;

    while (!Q.empty()) {
      auto [x, y] = Q.front();
      Q.pop();

      for (int i = 0; i < 4; ++i) {
        int a = x + dx[i];
        int b = y + dy[i];
        if (!sea.isValid(a, b, s) || visited[a][b] != -1) {
          continue;
        }

        visited[a][b] = visited[x][y] + 1;
        Q.emplace(a, b);
      }
    }

    for (int x = 1; x <= n; ++x) {
      for (int y = 1; y <= m; ++y) {
        if (visited[x][y] != -1) {
          if (sea.val[x][y] > 0) {
            val = sea.val[x][y];
          } else {
            val = 0;
          }

          // re-evaluate shield value
          if (sea.at[x][y] == "S") {
            if (s) val = 0;
          }

          // add bonus for each adjacent gold
          for (int i = 0; i < 4; ++i) {
            int a = x + dx[i];
            int b = y + dy[i];
            if (!sea.isValid(a, b, s)) {
              continue;
            }
            if (sea.val[a][b] > 0 && (!s || sea.at[a][b] != "S")) {
              val += 0.7 * sea.val[a][b];
            }
          }

          if (dist(x, y, _u, _v) <= visited[x][y]) {
            val = val * 2 / 3;
          }

          val = f(val + 1, visited[x][y] + 1);

          // accumulate table
          int i1 = max(1, x - range), j1 = max(1, y - range);
          int i2 = min(n, x + range), j2 = min(m, y + range);
          table[i1][j1] += val;
          table[i1][j2 + 1] -= val;
          table[i2 + 1][j1] -= val;
          table[i2 + 1][j2 + 1] += val;
        } else
          val = 0;
      }
    }

    if (k > totalTurn / 2) {
      sea.val[n / 2 + 1][m / 2 + 1] = 0;
    }

    int x = 1, y = 1;
    val = -INF;

    for (int i = 1; i <= n; ++i) {
      for (int j = 1; j <= m; ++j) {
        table[i][j] += table[i - 1][j] + table[i][j - 1] - table[i - 1][j - 1];

        if (table[i][j] > val) {
          val = table[i][j], x = i, y = j;
        }
      }
    }

    return make_tuple(val, x, y);
  }

  void MinimaxAgent::Input() {
    n = Store::M;
    m = Store::N;
    k = rootState.turnLeft;
    myShip.x = rootState.pos[0].x + 1;
    myShip.y = rootState.pos[0].y + 1;
    enemyShip.x = rootState.pos[1].x + 1;
    enemyShip.y = rootState.pos[1].y + 1;
    myShip.g = rootState.gold[0];
    myShip.s = rootState.hasShield[0];
    sea.Read(n, m);
  }

  void MinimaxAgent::Load() {
    lastMove = -1;
    if (myShip.x != 0) {
      totalTurn = Store::K;
      for (int i = 0; i < 4; ++i) {
        int a = rootState.lastPos[0].x + dx[i];
        int b = rootState.lastPos[0].y + dy[i];
        if (Position(a, b) == rootState.pos[0]) {
          lastMove = i;
          break;
        }
      }
      enemyShip.g = rootState.gold[1];
      enemyShip.s = rootState.hasShield[1];
    } else {
      totalTurn = k;
      enemyShip.g = 0, enemyShip.s = 0;
    }

    myShip.score = 0;
    enemyShip.score = 0;
  }

  void MinimaxAgent::MakeMove() {
    ofstream fout("MOVE.OUT");

    if (myShip.x == 0) {
      // First move
      auto [g, x, y] = Heuristic::BestPlace(sea, myShip, enemyShip, min(max(n, m) / 4, 4), k, totalTurn);
      myShip.Place(x, y);
      fout << myShip.x << ' ' << myShip.y << std::endl;
      return;
    }

    auto [score, dir] = MaxNode(-INF, INF, k, 0);
    if (dir != -1) myShip.Move(dir);
    fout << myShip.x << ' ' << myShip.y << std::endl;

    lastMove = dir;
  }

  pair<score_t, int> MinimaxAgent::MaxNode(score_t alpha, score_t beta, int turn, int depth) {
    _mx[depth] = myShip.x, _my[depth] = myShip.y, _mg[depth] = myShip.g, _ms[depth] = myShip.s;

    // check terminal state when 2 ships collide
    if ((depth > 0 && myShip.x == enemyShip.x && myShip.y == enemyShip.y) ||
        (depth > 0 && _mx[depth - 2] == enemyShip.x && _my[depth - 2] == enemyShip.y &&
         myShip.x == _ex[depth - 1] && myShip.y == _ey[depth - 1])) {
      auto [myShipScore, mx, my] = Heuristic::HighestHeat(sea, myShip, enemyShip, bval(2, 3, turn / 2), turn, totalTurn, true);
      myShipScore += myShip.score;

      if (_mg[depth - 2] <= _eg[depth - 1]) {
        return make_pair(-INF + myShipScore, -1);
      }
    }

    // check terminal state
    if (turn == 0 || (turn <= totalTurn / 2 && abs(myShip.g - enemyShip.g) > 20)) {
      score_t score = myShip.g > enemyShip.g ? INF : (-INF + 5);
      if (score < 0) {
        score += myShip.score;
      } else {
        score += myShip.score - enemyShip.score;
      }
      return make_pair(score, -1);
    }

    // reach max depth, try searching farther using heuristic
    if (depth >= MAX_DEPTH) {
      auto [mg, mx, my] = Heuristic::HighestHeat(sea, myShip, enemyShip, bval(2, 3, turn / 2), turn, totalTurn, true);
      auto [eg, ex, ey] = Heuristic::HighestHeat(sea, enemyShip, myShip, bval(2, 3, turn / 2), turn, totalTurn, true);

      score_t myShipScore = myShip.score + mg;
      score_t enemyShipScore = enemyShip.score + eg;

      return make_pair(myShipScore - enemyShipScore, -1);
    }

    // gen treasure
    if (turn == totalTurn / 2) {
      sea.val[n / 2 + 1][m / 2 + 1] = abs(myShip.g - enemyShip.g) * 3 / 4;
    }

    int sz, bestDir;
    score_t v, r, bestScore;
    vector<pair<score_t, int>> cand;

    auto [g, x, y] = Heuristic::HighestHeat(sea, myShip, enemyShip, bval(2, 3, turn / 2), turn, totalTurn, false);
    for (int i = 0; i < 4; ++i) {
      myShip.Move(i);
      cand.emplace_back(dist(myShip.x, myShip.y, x, y), i);
      myShip.Move(i ^ 1);
    }

    sort(cand.begin(), cand.end());

    // avoid going back
    if (depth == 0) {
      for (int j = 0; j < 4; ++j) {
        if (cand[j].second == (lastMove ^ 1)) {
          swap(cand[j], cand[3]);
          break;
        }
      }
    } else {
      for (int j = 0; j < 4; ++j) {
        int i = cand[j].second;
        myShip.Move(i);
        if (myShip.x == _mx[depth - 2] && myShip.y == _my[depth - 2])
          swap(cand[j], cand[3]);
        myShip.Move(i ^ 1);
      }
    }

    bestScore = -INF;
    bestDir = -1;
    sz = depth < MAX_DEPTH ? 3 : 1;

    for (int j = 0; j < sz && bestScore < beta; ++j) {
      int i = cand[j].second;

      // save state
      myShip.Move(i);

      if (!sea.isValid(myShip.x, myShip.y, myShip.s)) {
        myShip.Move(i ^ 1);
        continue;
      }

      if ((sea.at[myShip.x][myShip.y] == "D" && myShip.s) ||  // danger grid
          (sea.at[myShip.x][myShip.y] == "S" && myShip.s)) {  // shield grid
        v = 0;
      } else {
        v = sea.val[myShip.x][myShip.y];
      }

      if (v > 0) {
        r = f(v * RATE, depth / 2 + 1);
      } else {
        r = 0;
      }
      if (sea.at[myShip.x][myShip.y] == "S" && !myShip.s) {
        myShip.s = true;
      }

      if (sea.at[myShip.x][myShip.y] != "S") myShip.g += v;
      myShip.score += r;
      if (sea.at[myShip.x][myShip.y] != "D")
        sea.val[myShip.x][myShip.y] -= v;

      auto [score, dir] = MinNode(max(bestScore, alpha), beta, turn, depth + 1);

      // restore state
      if (sea.at[myShip.x][myShip.y] != "D")
        sea.val[myShip.x][myShip.y] += v;
      if (sea.at[myShip.x][myShip.y] != "S") myShip.g -= v;
      myShip.score -= r;
      myShip.s = _ms[depth];
      myShip.Move(i ^ 1);

      if (score > bestScore) {
        bestScore = score;
        bestDir = i;
      }
    }

    // revert treasure
    if (turn == totalTurn / 2) {
      sea.val[n / 2 + 1][m / 2 + 1] = 0;
    }

    return {bestScore, bestDir};
  }

  pair<score_t, int> MinimaxAgent::MinNode(score_t alpha, score_t beta, int turn, int depth) {
    _ex[depth] = enemyShip.x, _ey[depth] = enemyShip.y, _eg[depth] = enemyShip.g, _es[depth] = enemyShip.s;

    int sz, bestDir;
    score_t v, r, bestScore;
    vector<pair<score_t, int>> cand;

    auto [g, x, y] = Heuristic::HighestHeat(sea, enemyShip, myShip, bval(2, 3, turn / 2), turn, totalTurn, false);
    for (int i = 0; i < 4; ++i) {
      enemyShip.Move(i);
      cand.emplace_back(dist(enemyShip.x, enemyShip.y, x, y), i);
      enemyShip.Move(i ^ 1);
    }

    sort(cand.rbegin(), cand.rend());

    if (depth > 1) {
      for (int j = 0; j < 4; ++j) {
        int i = cand[j].second;
        enemyShip.Move(i);
        if (enemyShip.x == _ex[depth - 2] && enemyShip.y == _ey[depth - 2])
          swap(cand[j], cand[3]);
        enemyShip.Move(i ^ 1);
      }
    }

    bestScore = INF;
    bestDir = -1;
    sz = depth < MAX_DEPTH ? 3 : 1;

    for (int j = 0; j < sz && bestScore > alpha; ++j) {
      int i = cand[j].second;

      // save state
      enemyShip.Move(i);

      if (!sea.isValid(enemyShip.x, enemyShip.y, enemyShip.s)) {
        enemyShip.Move(i ^ 1);
        continue;
      }

      if ((sea.at[enemyShip.x][enemyShip.y] == "D" && enemyShip.s) ||  // danger grid
          (sea.at[enemyShip.x][enemyShip.y] == "S" && enemyShip.s)) {  // shield grid
        v = 0;
      } else {
        v = sea.val[enemyShip.x][enemyShip.y];
      }

      if (v > 0) {
        r = f(v * RATE, depth / 2 + 1);
      } else {
        r = 0;
      }
      if (sea.at[enemyShip.x][enemyShip.y] == "S" && !enemyShip.s) {
        enemyShip.s = true;
      }

      if (sea.at[enemyShip.x][enemyShip.y] != "S") enemyShip.g += v;
      enemyShip.score += r;
      if (sea.at[enemyShip.x][enemyShip.y] != "D")
        sea.val[enemyShip.x][enemyShip.y] -= v;

      auto [score, dir] = MaxNode(alpha, min(beta, bestScore), turn - 1, depth + 1);

      // restore state
      if (sea.at[enemyShip.x][enemyShip.y] != "D")
        sea.val[enemyShip.x][enemyShip.y] += v;
      if (sea.at[enemyShip.x][enemyShip.y] != "S") enemyShip.g -= v;
      enemyShip.score -= r;
      enemyShip.s = _es[depth];
      enemyShip.Move(i ^ 1);

      if (score < bestScore) {
        bestScore = score;
        bestDir = i;
      }
    }

    return {bestScore, bestDir};
  }

  void run() {
    MinimaxAgent agent;
    agent.Input();
    agent.Load();
    agent.MakeMove();
  }
}
