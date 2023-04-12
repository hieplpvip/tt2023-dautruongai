#include <bits/stdc++.h>

using namespace std;

typedef pair<int, int> ii;
typedef pair<double, int> di;

const double EPS = 1e-9;

// PRNG
mt19937_64 rng(chrono::high_resolution_clock::now().time_since_epoch().count());

int rand(int l, int r) {
  return uniform_int_distribution<int>(l, r)(rng);
}

ifstream fin("MAP.INP");
ofstream fout("MOVE.OUT");

int shield_value = 0;

int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, 1, 0, -1};

int n, m, k, org_k;
int x_1, y_1, x_2, y_2;
int s_1, g_1, s_2, g_2;
int middle_value;
char game_map[21][21];

int len[3][21][21];
di val[21][21];

bool can_go(int xx, int yy, int s) {
  return (xx > 0 && xx <= n && yy > 0 && yy <= m && (s || game_map[xx][yy] != 'D'));
}

int dist(int x1, int y1, int x2, int y2) {
  return abs(x1 - x2) + abs(y1 - y2);
}

void cal_len(int s, int id) {
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= m; j++) {
      len[id][i][j] = -1;
    }
  }

  queue<ii> q;
  q.emplace(x_1, y_1);

  len[id][x_1][y_1] = 1;
  while (!q.empty()) {
    auto [x, y] = q.front();
    q.pop();

    for (int i = 0; i < 4; ++i) {
      int nx = x + dx[i], ny = y + dy[i];

      if (!can_go(nx, ny, s)) continue;
      if (len[id][nx][ny] != -1) continue;

      len[id][nx][ny] = len[id][x][y] + 1;
      q.emplace(nx, ny);
    }
  }
}

double calculate_locality(int x, int y) {
  /*
  . . x . .
  . x x x .
  x x x x x
  . x x x .
  . . x . .
  */
  // calculate the sum of the x parts
  bool has_s = s_1;
  double sum = 0;
  for (int i = -n / 4; i <= n / 4; ++i) {
    for (int j = -m / 4; j <= m / 4; ++j) {
      if (abs(i) + abs(j) > max(n / 4, m / 4)) continue;
      if (!can_go(x + i, y + j, 0)) continue;
      if (game_map[x + i][y + j] == 'D') {
        if (!has_s) sum -= 1.0;
        continue;
      }

      int v = game_map[x + i][y + j] - '0';
      if (game_map[x + i][y + j] == 'S') {
        if (has_s) continue;
        has_s = true;
        v = (s_1 ? 0 : shield_value);
      }

      sum += 1.0 * v / (abs(i) + abs(j) + 1);
    }
  }
  return sum;
}

bool vis[16][16];
int dfs(int x, int y, int dep = 0) {
  if (!can_go(x, y, s_1) || vis[x][y] || game_map[x][y] == 'D' || game_map[x][y] == '0' || len[1][x][y] > k + 1 || dep >= 10) return 0;
  vis[x][y] = true;

  int res = 0;
  for (int i = 0; i < 4; i++) {
    int nx = x + dx[i], ny = y + dy[i];
    res = max(res, dfs(nx, ny, dep + 1));
  }
  vis[x][y] = false;
  int v = game_map[x][y] - '0';
  if (game_map[x][y] == 'S') {
    if (s_1)
      v = 0;
    else
      v = shield_value;
  }
  return v + res;
}

void cal_val() {
  cal_len(s_1, 1);
  cal_len(s_2, 2);

  int mx_len = 0;

  for (int i = 1; i <= n; ++i) {
    for (int j = 1; j <= m; ++j) {
      val[i][j] = di(-1000, -1000);
      mx_len = max(mx_len, len[1][i][j]);
    }
  }

  for (int v = mx_len; v >= 0; v--) {
    for (int i = 1; i <= n; ++i) {
      for (int j = 1; j <= m; ++j) {
        if (len[1][i][j] != v) continue;
        if (!can_go(i, j, s_1)) continue;

        double x = 0;

        if (game_map[i][j] != 'D') {
          // Value at current cell is highly prioritized
          if (i == (n + 1) / 2 && j == (m + 1) / 2) {
            x += middle_value * 100.0;
          } else if ('0' <= game_map[i][j] && game_map[i][j] <= '5')
            x += (game_map[i][j] - '0') * 1.75;

          // Find path with maximum gold
          int t = dfs(i, j);
          // if the path is closer to the other player, it is penalized (for the case when chasing the other player)
          if (len[2][i][j] < len[1][i][j])
            x += -1.25 * t / (2 * len[2][i][j] + 1);
          else  // rush to this path = ez win
            x += 1.25 * t / (2 * len[1][i][j] + 1);
        }

        if (game_map[i][j] == '0' || game_map[i][j] == 'D') {
          // Penalized for going to empty cell
          x += -3;
        }

        // penalize for going to the cell that distance exceed k
        // if (k <= 10 && len[1][i][j] <= k)
        //  x += -1.0 * len[1][i][j] / 2;  // add a small bonus when the game is almost over
        if (len[1][i][j] > k + 1) x += -1000;

        // x += - 1.0 * dist(x_1, y_1, i, j) * 3 / 4 + 1.0 * dist(x_2, y_2, i, j) / 3; // far from 1 is penalized or close to 2
        x += -1.0 * len[1][i][j] / 7;  // far from 1 is penalized
        // x += 1.0 * dist(x_2, y_2, i, j) / 7;   // far from 2 is bonus

        if (g_1 >= g_2)
          x += 1.75 * (len[1][i][j] + len[2][i][j] == len[1][x_2][y_2]);  // bonus moving to the cell that is closer to both 1 and 2

        if (dist(x_1, y_1, i, j) == 1 && ((x_2 == i && y_2 == j) || dist(x_2, y_2, i, j) == 1)) {  // move to this cell = :skull_emoji:
          // chi tranh tu sat khi chenh lech vang ko
          if (g_1 - g_2 <= 3)
            val[i][j] = di(-1000, -1000);
          // cout << '?' << g_1 << ' ' << g_2 << ' ' << i << ' ' << j << ' ' << val[i][j].first << ' ' << val[i][j].second << ' ' << '\n';
          continue;
        }

        x += 4.0 * (!can_go(x_1 - 1, y_1, s_1) + !can_go(x_1 + 1, y_1, s_1) + !can_go(x_1, y_1 - 1, s_1) + !can_go(x_1, y_1 + 1, s_1)) / 1 * (1 + (n <= 3) + (m <= 3));  // prioritize moving to inascessible
        x += calculate_locality(i, j);                                                                                                                                   // prioritize moving to the cell with high locality

        // Add a small bonus for moving to the center when k is close to half of org_k and closer to original than other person
        if ((abs(k - org_k / 2) <= max(m, n) / 3 || middle_value > 0) && len[1][(n + 1) / 2][(m + 1) / 2] < len[2][(n + 1) / 2][(m + 1) / 2]) {
          x += 1.0 / (dist(i, j, (n + 1) / 2, (m + 1) / 2) / 5) * (middle_value > 0 ? middle_value * 100.0 : abs(g_1 - g_2) * 2.75);
        }

        int add = 0;
        if (i == (n + 1) / 2 && j == (m + 1) / 2) {
          add = middle_value;
        } else if (game_map[i][j] == 'D') {
          add = 0;
        } else if (game_map[i][j] == 'S') {
          if (s_1)
            add = 0;
          else
            add = shield_value;
        } else {
          add = game_map[i][j] - '0';
        }
        if (x >= val[i][j].first) {
          val[i][j] = di(x, add);
        } else {
          val[i][j].second += add;
        }

        // cout << i << ' ' << j << ' ' << val[i][j].first << ' ' << val[i][j].second << '\n';

        for (int t = 0; t < 4; t++) {
          int nx = i + dx[t], ny = j + dy[t];
          if (!can_go(nx, ny, s_1)) continue;
          if (len[1][nx][ny] != v - 1) continue;

          val[nx][ny] = max(val[nx][ny], val[i][j]);
        }
      }
    }
  }
}

void cook() {
  fin >> n >> m >> k;
  fin >> x_1 >> y_1 >> x_2 >> y_2;
  fin >> g_1 >> s_1;

  g_2 = 100 - g_1;

  for (int i = 1; i <= n; ++i) {
    for (int j = 1; j <= m; ++j) {
      if (i == (n + 1) / 2 && j == (m + 1) / 2) {
        fin >> middle_value;
        g_2 -= middle_value;
      } else {
        fin >> game_map[i][j];
        if ('0' <= game_map[i][j] && game_map[i][j] <= '5') {
          g_2 -= game_map[i][j] - '0';
        }
      }
      if (game_map[i][j] == 'D') shield_value++;
    }
  }

  shield_value = 25.0 * shield_value / (n * m);

  if (x_1 == 0) {
    s_1 = true;  // chon lay vi en
    int best_val = INT_MIN;
    vector<ii> st;
    for (int i = 1; i <= n; i++) {
      for (int j = 1; j <= m; j++) {
        if (game_map[i][j] == 'D') continue;

        int tmp = dfs(i, j, 0) / 2 + calculate_locality(i, j) - dist(i, j, (n + 1) / 2, (m + 1) / 2);
        for (int t = 0; t < 4; t++) {
          int nx = i + dx[t], ny = j + dy[t];
          if (!can_go(nx, ny, 0)) continue;
          if (game_map[nx][ny] != '0') continue;

          if (tmp >= best_val) {
            if (tmp > best_val) {
              best_val = tmp;
              st.clear();
            }
            st.emplace_back(nx, ny);
          }
        }
      }
    }

    int ind = rand(0, (int)st.size() - 1);
    auto [best_x, best_y] = st[ind];
    fout << best_x << ' ' << best_y << '\n';

    ofstream sav("STATE.OUT");
    sav << k << endl;  // init so lan di chuyen
    sav.close();
    return;
  }

  ifstream load("STATE.OUT");
  int pre_x, pre_y;
  load >> org_k;
  load >> pre_x >> pre_y;
  load.close();

  cal_val();

  di best_val = di(-1000, -1000);
  int mx_val_in_cell = -1;
  int best_x = pre_x, best_y = pre_y;

  vector<ii> siuuuu;

  for (int i = 0; i < 4; i++) {
    int nx = x_1 + dx[i], ny = y_1 + dy[i];
    if (!can_go(nx, ny, s_1)) continue;
    // if (nx == pre_x && ny == pre_y) continue;  // ko di nguoc lai

    /*
    if (game_map[nx][ny] == '4' || game_map[nx][ny] == '5') {
      // Tat nao choi game
      siuuuu.emplace_back(nx, ny);
    }
    */
    // Thay vi tat nao choi game, add no vao val[nx][ny].first va prayge
    val[nx][ny].first += (('0' <= game_map[nx][ny] && game_map[nx][ny] <= '9') ? game_map[nx][ny] - '0' : 0) * 5.0;

    // cout << nx << ' ' << ny << ' ' << val[nx][ny].first << ' ' << val[nx][ny].second << '?' << '\n';
    // cout << best_val.first << ' ' << best_val.second << '!' << '\n';

    bool larger = (val[nx][ny].first - best_val.first > EPS) || (abs(val[nx][ny].first - best_val.first) < EPS && val[nx][ny].second > best_val.second);
    bool equal = abs(val[nx][ny].first - best_val.first) < EPS && (val[nx][ny].second == best_val.second);
    if (equal && best_x == pre_x && best_y == pre_y) {
      larger = true;
    }
    // cout << larger << ' ' << equal << '\n';
    if (larger || (equal && ((game_map[nx][ny] == 'D' ? 0 : game_map[nx][ny] - '0') > mx_val_in_cell))) {
      best_val = val[nx][ny];
      mx_val_in_cell = (game_map[nx][ny] == 'D' ? 0 : game_map[nx][ny] - '0');
      best_x = nx;
      best_y = ny;
    }
  }

  /*
  int mn_mv = -1;
  for (auto& [x, y] : siuuuu) {  // find the one with minimality movement
    int cnt_mv = (x == 1) + (x == n) + (y == 1) + (y == m);
    if (cnt_mv > mn_mv) {
      mn_mv = cnt_mv;
      best_x = x;
      best_y = y;
    }
  }
  */

  fout << best_x << " " << best_y << endl;

  ofstream sav("STATE.OUT");
  sav << org_k << endl;              // luu so lan di chuyen original
  sav << x_1 << " " << y_1 << endl;  // luu vi tri hien tai de no khong di nguoc ve
  sav.close();

  // calculate if player 2 use shield
  /*
  int cnt_s = 0;
  for (int i = 1; i <= n; ++i)
    for (int j = 1; j <= m; ++j)
      if (game_map[i][j] == 'S') ++cnt_s;

  if (cnt_s + s_1 < 2) s_2 = 1;
  else s_2 = 0;

  sav << s_2 << '\n';
  */
}

int main() {
  cook();
  return 0;
}
