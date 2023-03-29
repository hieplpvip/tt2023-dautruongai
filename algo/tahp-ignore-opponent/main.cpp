#include <bits/stdc++.h>
using namespace std;

const int dx[] = {1, -1, 0, 0};
const int dy[] = {0, 0, 1, -1};
const int INF = 1e6;
const int N = 20;
const double MAGIC = 0.65;

int m, n, k;
int mat[N][N], dist[N][N];
int our_x, our_y, opp_x, opp_y;
int total_gold, has_shield;
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
        mat[i][j] = has_shield ? 0 : -INF;
      } else if (s[0] == 'S') {
        mat[i][j] = 6;
        shield_pos.emplace_back(i, j);
      } else {
        mat[i][j] = stoi(s);
      }
    }
  }
}

void calc_all_dists(int sx, int sy) {
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
      if (is_valid(u, v) && dist[u][v] == -1 && mat[u][v] != -INF) {
        dist[u][v] = dist[x][y] + 1;
        qu.push({u, v});
      }
    }
  }
}

double heuristic(int nx, int ny) {
  if (!is_valid(nx, ny)) {
    return -INF;
  }
  int coin_gain = mat[nx][ny];
  mat[nx][ny] = 0;
  double heur = 0;
  calc_all_dists(nx, ny);
  for (int i = 1; i <= m; ++i) {
    for (int j = 1; j <= n; ++j) {
      if (mat[i][j] != -INF && dist[i][j] <= k) {
        heur += (double)mat[i][j] * pow(MAGIC, dist[i][j]);
      }
    }
  }

  mat[nx][ny] = coin_gain;
  return coin_gain + heur / 2;
}

void first_turn() {
  array<int, 2> next_move;
  double best = -INF;
  for (int x = 1; x <= m; ++x) {
    for (int y = 1; y <= n; ++y) {
      if (mat[x][y]) {
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
  print_move(next_move[0], next_move[1]);
  exit(0);
}

int main() {
  freopen("MOVE.OUT", "w", stdout);

  input();
  if (our_x == 0) {
    first_turn();
  } else {
    play();
  }

  return 0;
}
