// clang-format off
#include "testlib.h"
#include <bits/stdc++.h>
// clang-format on

using namespace std;

/*
Generate a map subject to the following constraints:
- Size of map is m x n. 1 <= m, n <= 15. m and n are odd.
- Number of turns is k. 1 <= k <= 100. k is even
- Each cell can be:
  + Nothing
  + Danger
  + Shield
  + Gold (1 - 5)
- Non-danger cells are connected.
- The center of the map always contains nothing.
- There are exactly 2 shields.
- Total gold is 100.
- The map is symmetric along horizontal, vertical, or diagonal/anti-diagonal axis.
*/

const int dx[] = {1, -1, 0, 0};
const int dy[] = {0, 0, 1, -1};
const int EMPTY = 0;
const int DANGER = -1;
const int SHIELD = -2;
const int N = 20;

bool check_danger(int a[N][N], int m, int n) {
  set<array<int, 2>> s;
  int numDanger = 0;
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      numDanger += (a[i][j] == DANGER);
    }
  }
  queue<array<int, 2>> qu;
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (a[i][j] != DANGER) {
        qu.push({i, j});
        s.insert({i, j});
        break;
      }
    }
    if (!qu.empty()) {
      break;
    }
  }
  if (qu.empty()) {
    return false;
  }
  while (!qu.empty()) {
    auto [x, y] = qu.front();
    qu.pop();
    for (int dir = 0; dir < 4; ++dir) {
      int u = x + dx[dir];
      int v = y + dy[dir];
      if (u < 0 || m <= u || v < 0 || n <= v || a[u][v] == DANGER) {
        continue;
      }
      if (!s.count({u, v})) {
        qu.push({u, v});
        s.insert({u, v});
      }
    }
  }
  // cerr << numDanger << ' ' << s.size() << ' ' << s.size() + numDanger << endl;
  return (numDanger + 5 <= m * n && (int)s.size() + numDanger == m * n);
}

bool check_center(int a[N][N], int m, int n) {
  return (a[m / 2][n / 2] == EMPTY);
}

void gen_danger_cells(int a[N][N], int m, int n, double rate) {
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (i > m / 2) {
        a[i][j] = a[2 * (m / 2) - i][j];
        continue;
      }
      a[i][j] = (rnd.next((double)0, (double)1) < rate ? DANGER : EMPTY);
      // cerr << rnd.next((double) 0, (double) 1) << ' ' << rate << endl;
    }
  }
}

bool check_symmetric(int a[N][N], int m, int n) {
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (a[i][j] != a[2 * (m / 2) - i][j]) {
        return false;
      }
    }
  }
  return true;
}

bool check_coins(int a[N][N], int m, int n) {
  int sum = 0;
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (a[i][j] > 0) {
        sum += a[i][j];
      }
    }
  }
  // cerr << "sum coins = " << sum << endl;
  return (sum == 100);
}

bool gen_coins(int a[N][N], int m, int n, int coin_bias) {
  int total = 100;
  vector<array<int, 2>> side_cells, center_cells;
  for (int i = 0; i < m / 2; ++i) {
    for (int j = 0; j < n; ++j) {
      if (a[i][j] == EMPTY) {
        side_cells.push_back({i, j});
      }
    }
  }
  for (int j = 0; j < n; ++j) {
    if (a[m / 2][j] == EMPTY && j != n / 2) {
      center_cells.push_back({m / 2, j});
    }
  }
  shuffle(side_cells.begin(), side_cells.end());
  shuffle(center_cells.begin(), center_cells.end());
  while (total > 0) {
    if (side_cells.empty() && center_cells.empty()) {
      return false;
    }
    if (total % 2 && center_cells.empty()) {
      return false;
    }
    int val = rnd.wnext(1, min(total, 5), coin_bias);
    if (rnd.next(0, 9) <= 8 && total >= 2 * val) {
      auto [x, y] = side_cells.back();
      side_cells.pop_back();
      a[x][y] = val;
      a[2 * (m / 2) - x][y] = val;
      total -= 2 * val;
    } else {
      auto [x, y] = center_cells.back();
      center_cells.pop_back();
      a[x][y] = val;
      total -= val;
    }
  }
  return true;
}

bool gen_shield(int a[N][N], int m, int n) {
  vector<array<int, 2>> candidates;
  for (int i = 0; i < m / 2; ++i) {
    for (int j = 0; j < n; ++j) {
      if (a[i][j] == EMPTY) {
        candidates.push_back({i, j});
      }
    }
  }
  if (candidates.size() == 0) {
    return false;
  }
  shuffle(candidates.begin(), candidates.end());
  a[candidates[0][0]][candidates[0][1]] = SHIELD;
  a[2 * (m / 2) - candidates[0][0]][candidates[0][1]] = SHIELD;
  return true;
}

void print(int m, int n, int k, int a[N][N]) {
  bool trans = rnd.next(0, 1);
  if (trans) {
    swap(m, n);
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < i; ++j) {
        swap(a[i][j], a[j][i]);
      }
    }
  }
  println(m, n, k);
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (a[i][j] == SHIELD) {
        cout << "S";
      } else if (a[i][j] == DANGER) {
        cout << "D";
      } else {
        cout << a[i][j];
      }
      cout << " \n"[j == n - 1];
    }
  }
}

int main(int argc, char** argv) {
  registerGen(argc, argv, 1);

  int m = opt<int>("m", 15);
  int n = opt<int>("n", 15);
  int k = opt<int>("k", 50);
  int coin_bias = opt<int>("coin_bias", 0);
  double danger_rate = opt<double>("danger_rate");

  while (true) {
    int a[N][N];
    gen_danger_cells(a, m, n, danger_rate);
    if (!check_danger(a, m, n) || !check_center(a, m, n) || !check_symmetric(a, m, n)) {
      continue;
    }
    if (!gen_shield(a, m, n)) {
      continue;
    }
    if (!gen_coins(a, m, n, coin_bias)) {
      continue;
    }
    ensure(check_center(a, m, n));
    ensure(check_danger(a, m, n));
    ensure(check_symmetric(a, m, n));
    ensure(check_coins(a, m, n));
    print(m, n, k, a);
    break;
  }

  return 0;
}
