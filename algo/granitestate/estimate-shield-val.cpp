#include <bits/stdc++.h>
#include "constant.h"
using namespace std;

namespace estimate_shield_val {
  const int dx[] = {1, -1, 0, 0};
  const int dy[] = {0, 0, 1, -1};
  const int INF = 1e6;
  const int N = 20;
  double MAGIC = 0.66;  // default

  const int DANGER = -1;
  const int EMPTY = 0;
  const int SHIELD = 6;

  int m, n, k;
  int mat[N][N], dist[N][N];
  double weight[N][N];
  int our_x, our_y, opp_x, opp_y;
  int total_gold, has_shield, rem_coins;
  vector<pair<int, int>> shield_pos;

  inline bool is_valid(int x, int y) {
    return 1 <= x && x <= m && 1 <= y && y <= n;
  }

  inline void print_move(int x, int y) {
    cout << x << ' ' << y << '\n';
    cout.flush();
  }

  void input() {
    freopen("MAP.INP", "r", stdin);
    cin >> m >> n >> k;
    cin >> our_x >> our_y >> opp_x >> opp_y;
    cin >> total_gold >> has_shield;

    string s;
    for (int i = 1; i <= m; ++i) {
      for (int j = 1; j <= n; ++j) {
        cin >> s;
        if (s[0] == 'D') {
          mat[i][j] = has_shield ? EMPTY : DANGER;
        } else if (s[0] == 'S') {
          mat[i][j] = SHIELD;
          shield_pos.emplace_back(i, j);
        } else {
          mat[i][j] = stoi(s);
          rem_coins += mat[i][j];
        }
      }
    }
  }

  void calc_all_dists(int sx, int sy, bool shield = false) {
    for (int i = 1; i <= m; ++i) {
      for (int j = 1; j <= n; ++j) {
        dist[i][j] = -1;
      }
    }
    queue<array<int, 2>> qu;
    dist[sx][sy] = 0;
    qu.push({sx, sy});
    while (!qu.empty()) {
      auto [x, y] = qu.front();
      qu.pop();
      for (int dir = 0; dir < 4; ++dir) {
        int u = x + dx[dir];
        int v = y + dy[dir];
        if (is_valid(u, v) && dist[u][v] == -1 && (shield || mat[u][v] != DANGER)) {
          dist[u][v] = dist[x][y] + 1;
          qu.push({u, v});
        }
      }
    }
  }

  void calc_magic() {
    double val = 0;
    int cnt = 0;
    for (int i = 1; i <= m; ++i) {
      for (int j = 1; j <= n; ++j) {
        if (mat[i][j] != EMPTY && mat[i][j] != SHIELD && mat[i][j] != DANGER) {
          calc_all_dists(i, j, true);
          ++cnt;
          for (int x = 1; x <= m; ++x) {
            for (int y = 1; y <= n; ++y) {
              if (i == x && j == y) {
                continue;
              }
              if (mat[x][y] != EMPTY && mat[x][y] != SHIELD && mat[x][y] != DANGER) {
                val += (double)1 / (dist[x][y] * dist[x][y]);
              }
            }
          }
        }
      }
    }
    // cerr << "calc magic: " << val / (cnt * (cnt - 1) / 2) << endl;
    MAGIC += val / (cnt * (cnt - 1) / 2);
  }

  double heuristic(int nx, int ny, bool shield = false) {
    if (!is_valid(nx, ny) || mat[nx][ny] == DANGER) {
      return -INF;
    }
    double coin_gain = weight[nx][ny];
    weight[nx][ny] = 0;
    double heur = 0;
    calc_all_dists(nx, ny, shield);
    for (int i = 1; i <= m; ++i) {
      for (int j = 1; j <= n; ++j) {
        if (mat[i][j] != DANGER && dist[i][j] <= k) {
          heur += (double)weight[i][j] * pow(MAGIC, dist[i][j]);
        }
      }
    }

    weight[nx][ny] = coin_gain;
    return coin_gain + heur / 2;
  }

  double calc_shield_weight(int x, int y) {
    return (1.02 - heuristic(x, y, false) / heuristic(x, y, true)) * rem_coins;
  }

  void calc_weights() {
    for (int i = 1; i <= m; ++i) {
      for (int j = 0; j <= n; ++j) {
        if (mat[i][j] == EMPTY) {
          weight[i][j] = 0;
        } else if (mat[i][j] == DANGER) {
          weight[i][j] = 0;
        } else if (mat[i][j] == SHIELD) {
          weight[i][j] = 0;  // calc_shield_weight(i, j);
        } else {
          weight[i][j] = mat[i][j];
        }
      }
    }

    if (has_shield) {
      return;
    }
    vector<double> shield_weights;
    for (auto [x, y] : shield_pos) {
      shield_weights.push_back(calc_shield_weight(x, y));
    }
    for (int i = 0; i < (int)shield_pos.size(); ++i) {
      // cerr << "shield: " << shield_weights[i] << endl;
      weight[shield_pos[i].first][shield_pos[i].second] = shield_weights[i];
    }
  }

  void first_turn() {
    array<int, 2> next_move;
    double best = -INF;
    for (int x = 1; x <= m; ++x) {
      for (int y = 1; y <= n; ++y) {
        if (mat[x][y] != EMPTY) {
          continue;
        }
        double val = heuristic(x, y);
        if (val > best) {
          best = val;
          next_move = {x, y};
        }
      }
    }
    print_move(next_move[0], next_move[1]);
    exit(0);
  }

  void play() {
    array<int, 2> next_move = {our_x, our_y};
    double best = heuristic(our_x, our_y);
    for (int dir = 0; dir < 4; ++dir) {
      int u = our_x + dx[dir];
      int v = our_y + dy[dir];
      double val = heuristic(u, v);
      if (val > best) {
        best = val;
        next_move = {u, v};
      }
    }
    // cerr << "best : " << best << endl;
    print_move(next_move[0], next_move[1]);
    exit(0);
  }

  int main() {
    freopen("MOVE.OUT", "w", stdout);

    input();
    calc_weights();
    calc_magic();
    if (our_x == 0) {
      first_turn();
    } else {
      play();
    }

    ofstream sav("STATE.OUT");
    sav << ALGO_ESTIMATE_SHIELD_VAL << endl;
    sav.close();

    return 0;
  }
}
