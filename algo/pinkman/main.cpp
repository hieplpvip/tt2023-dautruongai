#include <bits/stdc++.h>

using namespace std;

#define MAXN 20
#define INF 100000000
#define NDIR 4
#define MAX_DEPTH 10
#define RANGE 50
#define KSEED 10
#define EZONE 2
#define SHIELD_SCORE 10

#define dist(a, b, c, d) (abs((a) - (c)) + abs((b) - (d)))

const int
    dx[] = {-1, 1, 0, 0},
    dy[] = {0, 0, -1, 1};

ifstream inp, sav;
ofstream out;

// PRNG
mt19937_64 rng(chrono::high_resolution_clock::now().time_since_epoch().count());

class Random
{
public:
  static int rand(int l, int r)
  {
    return uniform_int_distribution<int>(l, r)(rng);
  }

  static int rand(int n)
  {
    return rand(0, n - 1);
  }
};

class Map
{
public:
  Map() {}

  void Read(int n, int m)
  {
    this->n = n;
    this->m = m;
    for (int i = 1; i <= n; ++i)
      for (int j = 1; j <= m; ++j)
      {
        inp >> at[i][j];

        if (at[i][j] == "D")
          val[i][j] = -INF;
        else if (at[i][j] == "S")
          val[i][j] = SHIELD_SCORE;
        else
          val[i][j] = stoi(at[i][j]);
      }
  }

  bool isValid(int x, int y, int s)
  {
    return x >= 1 && x <= n && y >= 1 && y <= m && (s || at[x][y] != "D");
  }

  int n, m;
  string at[MAXN][MAXN];
  int val[MAXN][MAXN];
};

class Ship
{
public:
  Ship() {}

  void Read()
  {
    inp >> x >> y;
  }

  void ReadInfo()
  {
    // inp >> g >> s;
    inp >> s >> g;
  }

  void Move(int dir)
  {
    x += dx[dir];
    y += dy[dir];
  }

  void Place(int x, int y)
  {
    this->x = x;
    this->y = y;
  }

  void Print()
  {
    out << x << " " << y << endl;
  }

  int x, y, g, s;
};

class Heuristic
{
private:
  // Get the total (gold - dist) in the range (choose 'lim' highest values)
  // Ignoring gold in the enemy zone (dist(enemyShip, pos) <= EZONE)
  static int bfs(Map &sea, int u, int v, int _u, int _v, int s, int lim, int range)
  {
    vector<int> gold;

    queue<pair<int, int>> Q;
    vector<vector<int>> visited(sea.n + 1, vector<int>(sea.m + 1, -1));

    Q.push({u, v});
    visited[u][v] = 0;

    while (!Q.empty())
    {
      u = Q.front().first;
      v = Q.front().second;
      Q.pop();

      if (visited[u][v] < dist(u, v, _u, _v) || dist(u, v, _u, _v) > EZONE)
        gold.push_back(sea.val[u][v] - visited[u][v]);

      for (int i = 0; i < NDIR; ++i)
      {
        int a = u + dx[i];
        int b = v + dy[i];
        if (!sea.isValid(a, b, s) || visited[a][b] != -1)
          continue;

        visited[a][b] = visited[u][v] + 1;
        if (visited[a][b] < range)
          Q.push({a, b});
      }
    }

    sort(gold.rbegin(), gold.rend());
    int res = 0;
    lim = min(lim, (int)gold.size());
    for (int i = 0; i < lim; ++i)
      res += gold[i];

    return res;
  }

  // Return value of the first (gold) reached
  // Ignore gold if enemy can come first
  static int bfs2(Map &sea, int u, int v, int _u, int _v, int s, int lim)
  {
    if (!sea.isValid(u, v, s))
      return -INF;

    queue<pair<int, int>> Q;
    vector<vector<int>> visited(sea.n + 1, vector<int>(sea.m + 1, -1));

    Q.push({u, v});
    visited[u][v] = 0;

    while (!Q.empty())
    {
      u = Q.front().first;
      v = Q.front().second;
      Q.pop();

      if (sea.val[u][v] > 0 && visited[u][v] <= dist(u, v, _u, _v))
        return sea.val[u][v];

      for (int i = 0; i < NDIR; ++i)
      {
        int a = u + dx[i];
        int b = v + dy[i];
        if (!sea.isValid(a, b, s) || visited[a][b] != -1)
          continue;

        visited[a][b] = visited[u][v] + 1;
        Q.push({a, b});
      }
    }

    return -INF;
  }

public:
  // (1) Find position where the ship can be placed to maximize the number of point in the range of RANGE
  // (2) Select 3 candidate positions and choose random to avoid conciding enemy's move
  static pair<int, int> FirstPlace(Map &sea)
  {
    vector<pair<int, pair<int, int>>> cand;

    int n = sea.n, m = sea.m;
    for (int i = 1; i <= n; ++i)
      for (int j = 1; j <= m; ++j)
      {
        if (sea.at[i][j] != "0")
          continue;

        int gold = bfs(sea, i, j, -INF, -INF, 0, INF, RANGE);
        cand.push_back({gold, {i, j}});
      }

    sort(cand.rbegin(), cand.rend());
    return cand[Random::rand(3)].second;
  }

  // TODO: Evaluate treasure
  // (1) Select KSEED posittions having the highest gold in the range RANGE
  // (2) Score is the difference between myShipScore and enemyShipScore
  // (3) If myShip drown (two ship collide or meet barrier), return -INF
  static int Evaluate(Map &sea, const Ship &myShip, const Ship &enemyShip)
  {
    if (!sea.isValid(myShip.x, myShip.y, myShip.s) || (myShip.x == enemyShip.x && myShip.y == enemyShip.y))
      return -INF;

    int myShipScore = bfs(sea, myShip.x, myShip.y, enemyShip.x, enemyShip.y, myShip.s, KSEED, INF);
    int enemyShipScore = sea.isValid(enemyShip.x, enemyShip.y, enemyShip.s)
                             ? bfs(sea, enemyShip.x, enemyShip.y, myShip.x, myShip.y, enemyShip.s, KSEED, INF)
                             : 0;

    // Give bonus score if the current grid is gold
    if (sea.val[myShip.x][myShip.y] > 0)
      myShipScore += 1000;

    return myShipScore - enemyShipScore;
  }
};

class MinimaxAgent
{
private:
  int k;
  Ship myShip, enemyShip;
  Map sea;

  pair<int, int> MaxNode(int alpha, int beta, int depth = 0)
  {
    int v = Heuristic::Evaluate(sea, myShip, enemyShip);
    if (v >= beta || depth == MAX_DEPTH)
      return {v, -1};

    v = max(v, alpha);
    int bestDir = -1;

    for (int i = 0; i < NDIR && v <= beta; ++i)
    {
      myShip.Move(i);
      auto [score, dir] = MinNode(v, beta, depth + 1);
      myShip.Move(i ^ 1);

      if (score > -INF && bestDir == -1)
        bestDir = i;
      if (score > v)
      {
        v = score;
        bestDir = i;
      }
    }

    return {v, bestDir};
  }
  pair<int, int> MinNode(int alpha, int beta, int depth = 0)
  {
    int v = Heuristic::Evaluate(sea, myShip, enemyShip);
    if (v <= alpha || depth == MAX_DEPTH)
      return {v, -1};

    v = min(v, beta);
    int bestDir = -1;

    for (int i = 0; i < NDIR && v >= alpha; ++i)
    {
      enemyShip.Move(i);
      auto [score, dir] = MaxNode(alpha, v, depth + 1);
      enemyShip.Move(i ^ 1);

      if (score < v)
      {
        v = score;
        bestDir = i;
      }
    }

    return {v, bestDir};
  }

public:
  MinimaxAgent() {}

  void Input()
  {
    int n, m;
    inp >> n >> m >> k;
    myShip.Read();
    enemyShip.Read();
    myShip.ReadInfo();
    sea.Read(n, m);
  }

  void MakeMove()
  {
    if (myShip.x == 0) // First move
    {
      auto [x, y] = Heuristic::FirstPlace(sea);
      myShip.Place(x, y);
      myShip.Print();
      return;
    }

    auto [score, dir] = MaxNode(-INF, INF);
    if (dir != -1)
      myShip.Move(dir);
    myShip.Print();
  }
};

int main()
{
  inp.open("MAP.INP");
  sav.open("STATE.OUT");
  out.open("MOVE.OUT");

  MinimaxAgent agent;
  agent.Input();
  agent.MakeMove();

  inp.close();
  sav.close();
  out.close();
}