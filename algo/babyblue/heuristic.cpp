#include "heuristic.h"

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
      score_t gold = Evaluate(sea, myShip, enemyShip, i, j, range, k, totalTurn);
      cand.emplace_back(gold, i, j);
    }
  }

  sort(cand.rbegin(), cand.rend());

  int krand = KRAND;
  while (krand > 1 && get<0>(cand[krand - 1]) <= -INF + 5) krand--;
  return cand[Random::rand(krand)];
}

// TODO: Evaluate treasure
// (1) Select KSEED positions having the highest gold in the range RANGE
// (2) Score is the difference between myShipScore and enemyShipScore
// (3) If myShip drown (two ship collide or meet barrier), return -INF
score_t Heuristic::Evaluate(Map &sea, Ship &myShip, Ship &enemyShip, int x, int y, int range, int k, int totalTurn) {
  if (!sea.isValid(myShip.x, myShip.y, myShip.s))
    return -INF;

  score_t val;
  int n = sea.n, m = sea.m;
  int u = myShip.x, v = myShip.y, s = myShip.s;
  int _u = enemyShip.x, _v = enemyShip.y, _s = enemyShip.s;
  // int range = (n + m) / 5;

  vector<score_t> score;

  queue<pair<int, int>> Q;
  vector<vector<int>> visited(n + 1, vector<int>(m + 1, -1));

  if (k > totalTurn / 2) {
    sea.val[n / 2 + 1][m / 2 + 1] = sqr(myShip.g - enemyShip.g + 1) / sqr(k - totalTurn / 2 + 3);
  }

  Q.emplace(x, y);
  visited[x][y] = 0;

  while (!Q.empty()) {
    auto [x, y] = Q.front();
    Q.pop();

    if (sea.val[x][y] > 0) {
      val = sea.val[x][y];

      // re-evaluate shield value
      if (sea.at[x][y] == "S") {
        if (s)
          val = 0;
        else
          s = 1;
      }

      // add bonus for each adjacent gold
      for (int i = 0; i < 4; ++i) {
        int a = x + dx[i];
        int b = y + dy[i];
        if (!sea.isValid(a, b, s)) {
          continue;
          if (sea.val[a][b] > 0) v += (sea.val[a][b] + 1) / 2;
        }
      }

      // if enemy can reach it first, then set to 1
      if (dist(x, y, _u, _v) <= min(EZONE, dist(x, y, u, v))) {
        val = 1;
      }

      val = sqr(val + 1) / (dist(x, y, u, v) + 1);
      score.push_back(val);
    }

    for (int i = 0; i < 4; ++i) {
      int a = x + dx[i];
      int b = y + dy[i];
      if (!sea.isValid(a, b, s) || visited[a][b] != -1) {
        continue;
      }

      visited[a][b] = visited[x][y] + 1;
      if (visited[a][b] < range) {
        Q.emplace(a, b);
      }
    }
  }

  if (k > totalTurn / 2) {
    sea.val[n / 2 + 1][m / 2 + 1] = 0;
  }

  sort(score.rbegin(), score.rend());
  score_t res = 0;
  int lim = (int)score.size();
  for (int i = 0; i < lim; ++i) {
    res += score[i];
  }

  // if can elinimate enemy in 1 step, bonus
  if (dist(u, v, _u, _v) <= 2 && myShip.g > enemyShip.g) {
    res += BONUS;
  }

  return res;
}
