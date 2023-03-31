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
      // score_t gold = Evaluate(sea, myShip, enemyShip, i, j, range, k, totalTurn);
      auto [gold, x, y] = HighestHeat(sea, myShip, enemyShip, range, k, totalTurn, false);
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
    sea.val[n / 2 + 1][m / 2 + 1] = 5 * ((myShip.g - enemyShip.g) * 3 / 4) / (k - totalTurn / 2 + 2);
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
        }
        if (sea.val[a][b] > 0) v += 0.2;
      }

      // if enemy can reach it first, then set to 1
      if (dist(x, y, _u, _v) <= min(EZONE, dist(x, y, u, v))) {
        val = val * 1 / 2;
      }

      // val = sqr(val + 1) / pow(dist(x, y, u, v) + 1, 1. / 3);
      val = f(val + 1, dist(x, y, u, v) + 1);
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

  // if can eliminate enemy in 1 step, bonus
  if (dist(u, v, _u, _v) <= 2 && myShip.g > enemyShip.g) {
    res += BONUS;
  }

  return res;
}

tuple<score_t, int, int> Heuristic::HighestHeat(Map &sea, Ship &myShip, Ship &enemyShip, int range, int k, int totalTurn, bool ignoreShield, bool debug) {
  int n = sea.n, m = sea.m;
  int u = myShip.x, v = myShip.y, s = myShip.s;
  int _u = enemyShip.x, _v = enemyShip.y, _s = enemyShip.s;
  score_t val;

  if (ignoreShield) {
    s = 1;
  }

  vector<vector<score_t>> table(n + 5, vector<score_t>(m + 5, 0));

  if (k > totalTurn / 2) {
    sea.val[n / 2 + 1][m / 2 + 1] = sqr(abs(myShip.g - enemyShip.g) * 3 / 4) / (k - totalTurn / 2 + 2);
    if (debug) {
      cerr << "Treasure: " << sea.val[n / 2 + 1][m / 2 + 1] << endl;
    }
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

        // if enemy can reach it first, then set to 1
        // if (dist(x, y, _u, _v) <= min(EZONE, visited[x][y])) {
        //   val = val * 2 / 3;
        // }

        if (dist(x, y, _u, _v) <= visited[x][y]) {
          val = val * 2 / 3;
        }

        if (debug && val < 0) cerr << "WTF_1 " << x << ' ' << y << '\n';

        // val = sqr(val + 1) / pow(visited[x][y] + 1, 1. / 3);
        val = f(val + 1, visited[x][y] + 1);

        if (debug && val < 0) cerr << "WTF_2 " << x << ' ' << y << '\n';

        // accumulate table
        int i1 = max(1, x - range), j1 = max(1, y - range);
        int i2 = min(n, x + range), j2 = min(m, y + range);
        table[i1][j1] += val;
        table[i1][j2 + 1] -= val;
        table[i2 + 1][j1] -= val;
        table[i2 + 1][j2 + 1] += val;
      } else
        val = 0;

      if (debug) cerr << setfill(' ') << setw(7) << setprecision(2) << fixed << val << ' ';
    }

    if (debug) cerr << endl;
  }

  if (debug) cerr << endl;

  if (k > totalTurn / 2) {
    sea.val[n / 2 + 1][m / 2 + 1] = 0;
  }

  score_t _val;
  int x = 1, y = 1, _x = 1, _y = 1;
  val = _val = -INF;

  for (int i = 1; i <= n; ++i) {
    for (int j = 1; j <= m; ++j) {
      table[i][j] += table[i - 1][j] + table[i][j - 1] - table[i - 1][j - 1];

      if (table[i][j] > val) {
        _val = val, _x = x, _y = y;
        val = table[i][j], x = i, y = j;
      } else if (table[i][j] > _val) {
        _val = table[i][j], _x = i, _y = j;
      }
    }
  }

  if (Random::rand(2)) {
    swap(val, _val);
    swap(x, _x);
    swap(y, _y);
  }

  // if can elinimate enemy in 1 step, bonus
  // if my gold is higher, the ship will chase the enemy, lmao
  // if (dist(u, v, _u, _v) <= 2 && myShip.g > enemyShip.g) {
  //   val += BONUS;
  // }

  if (debug) {
    cerr << "accumulated table:\n";
    for (int i = 1; i <= n; ++i) {
      for (int j = 1; j <= m; ++j) {
        cerr << setfill(' ') << setw(7) << setprecision(2) << fixed << table[i][j] << " ";
      }
      cerr << endl;
    }

    cerr << endl;

    cerr << "highest heat: " << val << " at (" << x << ", " << y << ")\n";
    cerr << "second heat: " << _val << " at (" << _x << ", " << _y << ")\n";

    cerr << endl;
  }

  return make_tuple(val, x, y);
}
