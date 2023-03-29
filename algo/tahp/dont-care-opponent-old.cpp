#include <bits/stdc++.h>
using namespace std;

const int SIZE = 20;
const int INF = 1e6;
const int dx[] = {1, -1, 0, 0};
const int dy[] = {0, 0, 1, -1};
const double RATE = 0.66;
const int SHIELD_VALUE = 9;

int m, n, k;
int mat[SIZE][SIZE], dist[SIZE][SIZE];
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
                mat[i][j] = SHIELD_VALUE;
                shield_pos.emplace_back(i, j);
            } else {
                mat[i][j] = stoi(s);
            }
        }
    }
}

bool inside(int x, int y) {
    return (1 <= x && x <= m && 1 <= y && y <= n);
}

void calc_all_dists(int start_x, int start_y) {
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            dist[i][j] = -1;
        }
    }
    queue<array<int, 2>> qu;
    dist[start_x][start_y] = 0;
    qu.push({start_x, start_y});
    while (!qu.empty()) {
        auto [x, y] = qu.front();
        qu.pop();
        for (int dir = 0; dir < 4; ++dir) {
            int u = x + dx[dir], v = y + dy[dir];
            if (inside(u, v) && dist[u][v] == -1 && mat[u][v] != -INF) {
                dist[u][v] = dist[x][y] + 1;
                qu.push({u, v});
            }
        }
    }
}

double eval(int next_x, int next_y) {
    int gold_gain = mat[next_x][next_y];
    int next_gold = total_gold + gold_gain;
    mat[next_x][next_y] = 0;
    calc_all_dists(next_x, next_y);
    double expected = 0;
    for (int x = 1; x <= m; ++x) {
        for (int y = 1; y <= n; ++y) {
            if (dist[x][y] != -1 && dist[x][y] <= k && mat[x][y] > 0) {
                expected += pow(RATE, dist[x][y]) * mat[x][y];
            }
        }
    }

    mat[next_x][next_y] = gold_gain;
    return next_gold + expected / 2.5; 
}

void first_turn() {
    // Find a starting positions next to shield
    array<int, 2> next_move = {-1, -1};
    double best = -INF;
    for (int next_x = 1; next_x <= m; ++next_x) {
        for (int next_y = 1; next_y <= n; ++next_y) {
            if (mat[next_x][next_y] != 0) {
                continue;
            }
            double value = eval(next_x, next_y);
            if (value > best) {
                best = value;
                next_move = {next_x, next_y};
            }
        }
    }
    print_move(next_move[0], next_move[1]);
    exit(0);
}

void play()
{
    array<int, 2> next_move = {our_x, our_y};
    double best = eval(our_x, our_y);
    for (int dir = 0; dir < 4; ++dir) {
        int next_x = our_x + dx[dir];
        int next_y = our_y + dy[dir];
        if (!inside(next_x, next_y)) {
            continue;
        }
        double value = eval(next_x, next_y);
        if (value > best) {
            best = value;
            next_move = {next_x, next_y};
        }
    }
    print_move(next_move[0], next_move[1]);
    exit(0);
}

int main()
{
    freopen("MOVE.OUT", "w", stdout);

    input();
    if (our_x == 0)
    {
        first_turn();
    }
    else
    {
        play();
    }

    return 0;
}
