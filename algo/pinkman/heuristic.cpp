#include "heuristic.h"

int Heuristic::bfs(Map &sea, int u, int v, int _u, int _v, int s, int lim, int range)
{
  vector<int> gold;

  queue<pair<int, int>> Q;
  vector<vector<int>> visited(sea.n + 1, vector<int>(sea.m + 1, -1));

  Q.emplace(u, v);
  visited[u][v] = 0;

  while (!Q.empty())
  {
    auto [u, v] = Q.front();
    Q.pop();

    if (sea.val[u][v] > 0 && (visited[u][v] < dist(u, v, _u, _v) || dist(u, v, _u, _v) > EZONE))
    {
      gold.push_back(sea.val[u][v] - visited[u][v] + BONUS_RATE);
    }

    for (int i = 0; i < NDIR; ++i)
    {
      int a = u + dx[i];
      int b = v + dy[i];
      if (!sea.isValid(a, b, s) || visited[a][b] != -1)
      {
        continue;
      }

      visited[a][b] = visited[u][v] + 1;
      if (visited[a][b] < range)
      {
        Q.emplace(a, b);
      }
    }
  }

  sort(gold.rbegin(), gold.rend());
  int res = 0;
  lim = min(lim, (int)gold.size());
  for (int i = 0; i < lim; ++i)
  {
    res += gold[i];
  }

  return res;
}

int Heuristic::bfs2(Map &sea, int u, int v, int _u, int _v, int s, int lim)
{
  if (!sea.isValid(u, v, s))
  {
    return -INF;
  }

  queue<pair<int, int>> Q;
  vector<vector<int>> visited(sea.n + 1, vector<int>(sea.m + 1, -1));

  Q.emplace(u, v);
  visited[u][v] = 0;

  while (!Q.empty())
  {
    auto [u, v] = Q.front();
    Q.pop();

    if (sea.val[u][v] > 0 && visited[u][v] <= dist(u, v, _u, _v))
    {
      return sea.val[u][v];
    }

    for (int i = 0; i < NDIR; ++i)
    {
      int a = u + dx[i];
      int b = v + dy[i];
      if (!sea.isValid(a, b, s) || visited[a][b] != -1)
      {
        continue;
      }

      visited[a][b] = visited[u][v] + 1;
      Q.emplace(a, b);
    }
  }

  return -INF;
}

pair<int, int> Heuristic::FirstPlace(Map &sea)
{
  vector<pair<int, pair<int, int>>> cand;

  int n = sea.n, m = sea.m;
  for (int i = 1; i <= n; ++i)
  {
    for (int j = 1; j <= m; ++j)
    {
      if (sea.at[i][j] != "0")
      {
        continue;
      }

      int gold = bfs(sea, i, j, -INF, -INF, 0, INF, RANGE);
      cand.push_back({gold, {i, j}});
    }
  }

  sort(cand.rbegin(), cand.rend());

  return cand[Random::rand(KRAND)].second;
}

// TODO: Evaluate treasure
// (1) Select KSEED positions having the highest gold in the range RANGE
// (2) Score is the difference between myShipScore and enemyShipScore
// (3) If myShip drown (two ship collide or meet barrier), return -INF
int Heuristic::Evaluate(Map &sea, const Ship &myShip, const Ship &enemyShip)
{
  if (!sea.isValid(myShip.x, myShip.y, myShip.s))
    return -INF;

  int myShipScore = bfs(sea, myShip.x, myShip.y, enemyShip.x, enemyShip.y, myShip.s, KSEED, INF);
  int enemyShipScore = sea.isValid(enemyShip.x, enemyShip.y, enemyShip.s)
                           ? bfs(sea, enemyShip.x, enemyShip.y, myShip.x, myShip.y, enemyShip.s, KSEED, INF)
                           : 0;

  // Give bonus score if the current grid is gold
  if (sea.val[myShip.x][myShip.y] > 0)
  {
    myShipScore += BONUS_RATE * BONUS_RATE * BONUS_RATE;
  }

  return myShipScore - enemyShipScore;
}
