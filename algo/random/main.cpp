#include <bits/stdc++.h>
using namespace std;
const int dx[] = {1, -1, 0, 0};
const int dy[] = {0, 0, 1, -1};

// Cell type
const int FREE = 0;
const int DANGER = -1;
const int SHIELD = -2;

// PRNG
mt19937_64 rng(chrono::high_resolution_clock::now().time_since_epoch().count());

int rand(int l, int r) {
  return uniform_int_distribution<int>(l, r)(rng);
}

int rand(int n) {
  return rand(0, n - 1);
}

int m, n, k;
int mat[20][20];
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
  cin >> m >> n >> k;
  cin >> our_x >> our_y >> opp_x >> opp_y;
  cin >> total_gold >> has_shield;

  string s;
  for (int i = 1; i <= m; ++i) {
    for (int j = 1; j <= n; ++j) {
      cin >> s;
      if (s[0] == 'D') {
        mat[i][j] = has_shield ? FREE : DANGER;
      } else if (s[0] == 'S') {
        mat[i][j] = SHIELD;
        shield_pos.emplace_back(i, j);
      } else {
        mat[i][j] = stoi(s);
      }
    }
  }
}

void first_turn() {
  // Find a starting positions next to shield
  vector<pair<int, int>> candidates;
  for (auto [x, y] : shield_pos) {
    for (int t = 0; t < 4; ++t) {
      int u = x + dx[t];
      int v = y + dy[t];
      if (is_valid(u, v) && mat[u][v] == FREE) {
        candidates.emplace_back(u, v);
      }
    }
  }

  if (candidates.empty()) {
    print_move(-1, -1);
  } else {
    auto [x, y] = candidates[rand(candidates.size())];
    print_move(x, y);
  }
  exit(0);
}

void random_play() {
  int gold_x = -1, gold_y = -1, gold_max = 0;
  int shield_x = -1, shield_y = -1;
  vector<pair<int, int>> free, danger;

  for (int t = 0; t < 4; ++t) {
    int u = our_x + dx[t];
    int v = our_y + dy[t];
    if (!is_valid(u, v)) continue;

    if (mat[u][v] == FREE) {
      free.emplace_back(u, v);
    } else if (mat[u][v] == DANGER) {
      danger.emplace_back(u, v);
    } else if (mat[u][v] == SHIELD) {
      shield_x = u;
      shield_y = v;
    } else if (mat[u][v] > gold_max) {
      gold_x = u;
      gold_y = v;
      gold_max = mat[u][v];
    }
  }

  if (shield_x != -1) {
    // Take the fucking shield
    print_move(shield_x, shield_y);
    exit(0);
  }

  if (gold_x != -1) {
    // Take gold
    print_move(gold_x, gold_y);
    exit(0);
  }

  if (!free.empty()) {
    auto [x, y] = free[rand(free.size())];
    print_move(x, y);
    exit(0);
  }

  print_move(-1, -1);
  exit(0);
}

int main() {
  assert(freopen("MAP.INP", "r", stdin) == stdin);
  assert(freopen("MOVE.OUT", "w", stdout) == stdout);

  input();
  if (our_x == 0) {
    first_turn();
  } else {
    random_play();
  }

#ifdef PRINT_MESSAGE
  std::cerr << "From Seno with love <3" << std::endl;
#endif

  return 0;
}
