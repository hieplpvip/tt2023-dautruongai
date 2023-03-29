#include "heuristic.h"

pair<int, int> Heuristic::FirstPlace(Map &sea, Ship &myShip, Ship &enemyShip) {
  vector<pair<int, pair<int, int>>> cand;

  int n = sea.n, m = sea.m;
  for (int i = 1; i <= n; ++i) {
    for (int j = 1; j <= m; ++j) {
      if (sea.at[i][j] != "0") {
        continue;
      }

      myShip.Place(i, j);
      enemyShip.Place(-INF, -INF);
      int gold = Evaluate(sea, myShip, enemyShip);
      cand.push_back({gold, {i, j}});
    }
  }

  sort(cand.rbegin(), cand.rend());

  return cand[Random::rand(KRAND)].second;
}

// TODO: Evaluate treasure
// (1) Select KSEED positions having the highest gold in the range RANGE
// (2) Score is the difference between myShipScore and enemyShipScore
// (3) If myShip drown (two ship collide or meet barrier), return -INF
int Heuristic::Evaluate(Map &sea, const Ship &myShip, const Ship &enemyShip) {
  if (!sea.isValid(myShip.x, myShip.y, myShip.s))
    return -INF;

  int val;
  int n = sea.n, m = sea.m;
  int u = myShip.x, v = myShip.y, s = myShip.s;
  int _u = enemyShip.x, _v = enemyShip.y, _s = enemyShip.s;
  int range = (n + m) / 2;

  vector<int> gold;

  queue<pair<int, int>> Q;
  vector<vector<int>> visited(sea.n + 1, vector<int>(sea.m + 1, -1));

  Q.emplace(u, v);
  visited[u][v] = 0;

  while (!Q.empty()) {
    auto [u, v] = Q.front();
    Q.pop();

    if (sea.val[u][v] > 0) {
      // if enemy can reach it first, then set to 1
      val = (dist(u, v, _u, _v) <= min(EZONE, visited[u][v])) ? 1 : sea.val[u][v];
      if (u == n / 2 + 1 && v == m / 2 + 1) val = max(val, 5);
      gold.push_back((val + 1) * (BONUS - visited[u][v]));
    }

    for (int i = 0; i < 4; ++i) {
      int a = u + dx[i];
      int b = v + dy[i];
      if (!sea.isValid(a, b, s) || visited[a][b] != -1) {
        continue;
      }

      visited[a][b] = visited[u][v] + 1;
      if (visited[a][b] < range) {
        Q.emplace(a, b);
      }
    }
  }

  sort(gold.rbegin(), gold.rend());
  int res = 0;
  int lim = min(KSEED, (int)gold.size());
  for (int i = 0; i < lim; ++i) {
    res += gold[i];
  }

  return res;
}
