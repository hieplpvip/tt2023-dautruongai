// clang-format off
#include "../testlib.h"
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

// FIXME: For now we just generate a random map and check if it satisfies the constraints.
// We should find a better way to do this (perhaps after implementing the agents)

const int dx[] = {1, -1, 0, 0};
const int dy[] = {0, 0, 1, -1};

class Generator {
private:
  int m, n, k;
  vector<vector<char>> a;

  void symmetrize() {
    /*
    "Symmetrize" the map in one of 4 ways:
    - Horizontal
    - Vertical
    - Main diagonal
    - Anti diagonal

    This is a fucking ugly hack.
    */

    int r;
    if (m == n) {
      r = rnd.next(0, 3);
    } else {
      r = rnd.next(0, 1);
    }
    if (r == 0) {
      for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
          a[i][j] = a[m - i - 1][j];
        }
      }
    } else if (r == 1) {
      for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
          a[i][j] = a[i][n - j - 1];
        }
      }
    } else if (r == 2) {
      for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
          a[i][j] = a[j][i];
        }
      }
    } else {
      for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
          a[i][j] = a[m - j - 1][n - i - 1];
        }
      }
    }
  }

  bool checkConnected() {
    int countDanger = 0;
    int start_x = -1, start_y = -1;
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        if (a[i][j] == 'D') {
          ++countDanger;
        } else {
          start_x = i;
          start_y = j;
        }
      }
    }
    if (start_x == -1) {
      return false;
    }

    int countSafe = 0;
    queue<pair<int, int>> q;
    vector<vector<bool>> visited(m, vector<bool>(n, false));
    visited[start_x][start_y] = true;
    q.emplace(start_x, start_y);

    while (!q.empty()) {
      auto [u, v] = q.front();
      q.pop();
      ++countSafe;

      for (int k = 0; k < 4; ++k) {
        int x = u + dx[k];
        int y = v + dy[k];
        if (0 <= x && x < m && 0 <= y && y < n && !visited[x][y] && a[x][y] != 'D') {
          visited[x][y] = true;
          q.emplace(x, y);
        }
      }
    }

    return countDanger + countSafe == m * n;
  }

  bool checkShield() {
    int countShield = 0;
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        if (a[i][j] == 'S') {
          ++countShield;
        }
      }
    }
    return countShield == 2;
  }

  bool checkGold() {
    int totalGold = 0;
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        if (isdigit(a[i][j])) {
          totalGold += a[i][j] - '0';
        }
      }
    }
    return totalGold == 100;
  }

public:
  bool generate(int m_hint, int n_hint, int k_hint) {
    if (m_hint != -1) {
      m = m_hint;
    } else {
      m = rnd.next(1, 15);
      if (m % 2 == 0) {
        ++m;
      }
    }

    if (n_hint != -1) {
      n = n_hint;
    } else {
      n = rnd.next(1, 15);
      if (n % 2 == 0) {
        ++n;
      }
    }

    if (k_hint != -1) {
      k = k_hint;
    } else {
      k = rnd.next(1, 100);
      if (k % 2 == 1) {
        ++k;
      }
    }

    ensuref(1 <= m && m <= 15, "m = %d is not in [1, 15]", m);
    ensuref(m % 2 == 1, "m = %d is not odd", m);
    ensuref(1 <= n && n <= 15, "n = %d is not in [1, 15]", n);
    ensuref(n % 2 == 1, "n = %d is not odd", n);
    ensuref(1 <= k && k <= 100, "k = %d is not in [1, 100]", k);
    ensuref(k % 2 == 0, "k = %d is not even", k);

    a.assign(m, vector<char>(n, '0'));

    // Generate a random and asymmetric map.
    // Each cell has 1/3 chance of being a danger cell, 2/3 chance of being a gold/empty cell.
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        a[i][j] = ((rnd.next(3) == 0) ? 'D' : ('0' + rnd.next(0, 5)));
      }
    }

    // Clear center cell
    a[m / 2][n / 2] = '0';

    // Place one shield. "symmetrize" should ensure that there are exactly 2 shields...
    {
      int x = rnd.next(0, m - 1);
      int y = rnd.next(0, n - 1);
      a[x][y] = 'S';
    }

    symmetrize();
    return checkConnected() && checkShield() && checkGold();
  }

  void print() {
    println(m, n, k);
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        cout << a[i][j] << " \n"[j == n - 1];
      }
    }
  }
};

int main(int argc, char** argv) {
  registerGen(argc, argv, 1);

  int m_hint = opt<int>("m", -1);
  int n_hint = opt<int>("n", -1);
  int k_hint = opt<int>("k", -1);

  Generator gen;
  do {
  } while (!gen.generate(m_hint, n_hint, k_hint));
  gen.print();

  return 0;
}
