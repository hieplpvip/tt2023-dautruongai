#include <bits/stdc++.h>

using namespace std;

typedef pair<int, int> ii;
typedef pair<double, int> di;

const double EPS = 1e-9;

ifstream fin("MAP.INP");
ofstream fout("MOVE.OUT");

int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, 1, 0, -1};

int n, m, k, org_k;
int x_1, y_1, x_2, y_2;
int s_1, g_1, s_2, g_2;
char game_map[21][21];

int len[3][21][21];
di val[21][21];

bool can_go(int xx, int yy, int s) {
  return (xx > 0 && xx <= n && yy > 0 && yy <= m && (s || game_map[xx][yy] != 'D'));
}

int dist(int x1, int y1, int x2, int y2) {
  return abs(x1 - x2) + abs(y1 - y2);
}

void cal_len(int x, int y, int s, int id) {
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= m; j++) {
      len[id][i][j] = -1;
    }
  }

  queue<ii> q;
  q.emplace(x_1, y_1);

  len[id][x_1][y_1] = 1;
  while (!q.empty()) {
    int x = q.front().first, y = q.front().second;
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
  for (int i = -n / 2; i <= n / 2; ++i) {
    for (int j = -m / 2; j <= m / 2; ++j) {
      if (abs(i) + abs(j) > 2) continue;
      if (!can_go(x + i, y + j, 0)) continue;
      if (game_map[x + i][y + j] == 'D') continue;

      int v = game_map[x + i][y + j] - '0';
      if (game_map[x + i][y + j] == 'S') {
        if (has_s) continue;
        has_s = true;
        v = (s_1 ? 0 : 4);
      }

      sum += 1.0 * v / (abs(i) + abs(j) + 1);
    }
  }
  return sum;
}

void cal_val() {
  cal_len(x_1, y_1, s_1, 1);
  cal_len(x_2, y_2, s_2, 2);

  int mx_len = 0;

  for (int i = 1; i <= n; ++i) {
    for (int j = 1; j <= m; ++j) {
      mx_len = max(mx_len, len[1][i][j]);
    }
  }

  for (int v = mx_len; v >= 0; v--) {
    for (int i = 1; i <= n; ++i) {
      for (int j = 1; j <= m; ++j) {
        if (len[1][i][j] != v) continue;
        if (!can_go(i, j, s_1)) continue;

        double x = 0;

        if (game_map[i][j] == 'S') {
          // Prioritize shield
          if (!s_1) x = 4;
        } else if (game_map[i][j] != 'D') {
          // Value at current cell is highly prioritized
          x = (game_map[i][j] - '0') * 1.75;
        }

        if (game_map[i][j] == '0' || game_map[i][j] == 'D') {
          // Penalized for going to empty cell
          x = -3;
        }

        // x += - 1.0 * dist(x_1, y_1, i, j) * 3 / 4 + 1.0 * dist(x_2, y_2, i, j) / 3; // far from 1 is penalized or close to 2
        x += -1.0 * dist(x_1, y_1, i, j) / 4;                              // far from 1 is penalized
        x += -1.0 * abs(dist(x_1, y_1, i, j) - dist(x_2, y_2, i, j)) / 4;  // prioritize moving to the cell that is closer to both 1 and 2

        x += 1.0 * (-(i > 1) - (i < n) - (j > 1) - (j < m)) / 2 * (1 + (n <= 3) + (m <= 3));  // prioritize moving to the corner
        x += calculate_locality(i, j);                                                        // prioritize moving to the cell with high locality

        // Add a small bonus for moving to the center when k is close to half of org_k
        if (abs(k - org_k / 2) <= 3) {
          x += -1.0 * dist(i, j, (n + 1) / 2, (m + 1) / 2) * (3.5 - abs(k - org_k / 2));
        }

        if (x >= val[i][j].first) {
          val[i][j] = di(x, (game_map[i][j] == 'D' ? 0 : game_map[i][j] - '0'));
        } else {
          val[i][j].second += (game_map[i][j] == 'D' ? 0 : game_map[i][j] - '0');
        }

        // cout << i << ' ' << j << ' ' << val[i][j].first << ' ' << val[i][j].second << '\n';

        for (int k = 0; k < 4; k++) {
          int nx = i + dx[k], ny = j + dy[k];
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

  for (int i = 1; i <= n; ++i) {
    for (int j = 1; j <= m; ++j) {
      fin >> game_map[i][j];
    }
  }

  if (x_1 == 0) {
    double best_val = -1;
    int best_x = -1, best_y = -1;

    for (int i = 1; i <= n; i++) {
      for (int j = 1; j <= m; j++) {
        if (game_map[i][j] != '0') continue;

        double v = calculate_locality(i, j) - 1.0 * dist(i, j, (n + 1) / 2, (m + 1) / 2) / 2;
        if (v > best_val) {
          best_val = v;
          best_x = i;
          best_y = j;
        }
      }
    }
    fout << best_x << ' ' << best_y << '\n';  // choose best first place

    ofstream sav("STATE.OUT");
    sav << k;  // init so lan di chuyen
    sav.close();
    return;
  }

  ifstream load("STATE.OUT");
  int pre_x, pre_y;
  load >> org_k;
  load >> pre_x >> pre_y;
  load.close();

  cal_val();

  di best_val = di(-1, -1);
  int mx_val_in_cell = -1;
  int best_x = pre_x, best_y = pre_y;

  vector<ii> siuuuu;

  for (int i = 0; i < 4; i++) {
    int nx = x_1 + dx[i], ny = y_1 + dy[i];
    if (!can_go(nx, ny, s_1)) continue;
    if (nx == pre_x && ny == pre_y) continue;  // ko di nguoc lai

    if (game_map[nx][ny] == '4' || game_map[nx][ny] == '5') {
      // Tat nao choi game
      siuuuu.emplace_back(nx, ny);
    }
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

  int mn_mv = -1;
  for (auto& [x, y] : siuuuu) {  // find the one with minimality movement
    int cnt_mv = (x == 1) + (x == n) + (y == 1) + (y == m);
    if (cnt_mv > mn_mv) {
      mn_mv = cnt_mv;
      best_x = x;
      best_y = y;
    }
  }

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
  // freopen("MAP.INP", "r", stdin);
  // freopen("MOVE.OUT", "w", stdout);

  srand(time(NULL));

  cook();

  return 0;
}
