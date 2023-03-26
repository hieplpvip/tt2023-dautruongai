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
- There are exactly 2 shields.
- The center of the map always contains nothing.
- The map is symmetric along horizontal, vertical, or diagonal/anti-diagonal axis.
*/

int n, m, k;
char a[31][31];

void symmetrize() {
  /*
  Symmetrize the map in one of 4 ways:
  - Horizontal
  - Vertical
  - Main diagonal
  - Anti diagonal
  */

  int r;
  if (m == n) {
    r = rnd.next(0, 3);
  } else {
    r = rnd.next(0, 1);
  }
  if (r == 0) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        a[i][j] = a[n - i - 1][j];
      }
    }
  } else if (r == 1) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        a[i][j] = a[i][m - j - 1];
      }
    }
  } else if (r == 2) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        a[i][j] = a[j][i];
      }
    }
  } else {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        a[i][j] = a[m - j - 1][n - i - 1];
      }
    }
  }
}

void genRaw() {
  // Tạo ngẫu nhiên một bản đồ không đối xứng, mỗi ô có tỉ lệ 1/3 là 'D' và 2/3 là '0' - '5'
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      a[i][j] = ((rnd.next(0, 9) < 3) ? 'D' : ('0' + rnd.next(0, 5)));

  // ô chính giữa = 0
  a[n / 2][m / 2] = '0';

  // chính xác 2 ô chứa khiên
  int x = rnd.next(0, n - 1), y = rnd.next(0, m - 1);
  a[x][y] = 'S';
  x = rnd.next(0, n - 1), y = rnd.next(0, m - 1);
  a[x][y] = 'S';
}

bool isConnected() {
  // Kiểm tra rằng mỗi ô không phải là vật cản có thể đi đến tất cả những ô khác không phải vật cản
  // Nếu không thì tạo lại bản đồ
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      if (a[i][j] == 'D')
        continue;
      vector<vector<bool>> visited(n, vector<bool>(m, false));
      queue<pair<int, int>> q;
      q.push({i, j});
      visited[i][j] = true;
      while (!q.empty()) {
        int x = q.front().first, y = q.front().second;
        q.pop();
        if (a[x][y] == 'D')
          continue;
        if (x > 0 && !visited[x - 1][y]) {
          visited[x - 1][y] = true;
          q.push({x - 1, y});
        }
        if (x < n - 1 && !visited[x + 1][y]) {
          visited[x + 1][y] = true;
          q.push({x + 1, y});
        }
        if (y > 0 && !visited[x][y - 1]) {
          visited[x][y - 1] = true;
          q.push({x, y - 1});
        }
        if (y < m - 1 && !visited[x][y + 1]) {
          visited[x][y + 1] = true;
          q.push({x, y + 1});
        }
      }

      for (int ii = 0; ii < n; ii++)
        for (int jj = 0; jj < m; jj++)
          if (a[ii][jj] != 'D' && !visited[ii][jj])
            return false;
    }
  }
  return true;
}

int main(int argc, char** argv) {
  registerGen(argc, argv, 1);

  m = rnd.next(1, 15);
  n = rnd.next(1, 15);
  if (m % 2 == 0) {
    ++m;
  }
  if (n % 2 == 0) {
    ++n;
  }
  println(m, n);

  while (true) {
    genRaw();
    symmetrize();
    if (isConnected()) {
      for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
          cout << a[i][j] << " \n"[j == n - 1];
        }
      }

      break;
    }
  }

  return 0;
}
