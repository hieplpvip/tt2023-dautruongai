#include "minimax.h"

MinimaxAgent::MinimaxAgent() {}

void MinimaxAgent::Input(ifstream &inp) {
  int n, m;
  inp >> n >> m >> k;
  myShip.Read(inp);
  enemyShip.Read(inp);
  myShip.ReadInfo(inp);
  sea.Read(inp, n, m);
}

void MinimaxAgent::Load(ifstream &sav) {
  int u, v;
  while (sav >> u >> v) {
    visited.insert(make_pair(u, v));
  }
}

void MinimaxAgent::Save(ofstream &sav) {
  for (auto &[x, y] : visited) {
    sav << x << ' ' << y << '\n';
  }
}

void MinimaxAgent::MakeMove(ofstream &out) {
  if (myShip.x == 0) {
    // First move
    auto [x, y] = Heuristic::FirstPlace(sea);
    myShip.Place(x, y);
    myShip.Print(out);
    return;
  }

  auto [score, dir] = MaxNode(-INF, INF);
  if (dir != -1) {
    myShip.Move(dir);
  }
  myShip.Print(out);

  if (sea.val[myShip.x][myShip.y] > 0) {
    visited.clear();
  } else {
    visited.insert(make_pair(myShip.x, myShip.y));
  }
}

pair<int, int> MinimaxAgent::MaxNode(int alpha, int beta, int depth) {
  int v = Heuristic::Evaluate(sea, myShip, enemyShip);
  if (v >= beta || depth == MAX_DEPTH) {
    return {v, -1};
  }

  v = max(v, alpha);
  int bestDir = -1;

  for (int i = 0; i < NDIR && v <= beta; ++i) {
    myShip.Move(i);
    auto [score, dir] = visited.find(make_pair(myShip.x, myShip.y)) != visited.end()
                            ? make_pair(-INF, -1)
                            : MinNode(v, beta, depth + 1);
    myShip.Move(i ^ 1);

    if (score > -INF && bestDir == -1) {
      bestDir = i;
    }
    if (score > v) {
      v = score;
      bestDir = i;
    }
  }

  return {v, bestDir};
}

pair<int, int> MinimaxAgent::MinNode(int alpha, int beta, int depth) {
  int v = Heuristic::Evaluate(sea, myShip, enemyShip);
  if (v <= alpha || depth == MAX_DEPTH) {
    return {v, -1};
  }

  v = min(v, beta);
  int bestDir = -1;

  for (int i = 0; i < NDIR && v >= alpha; ++i) {
    enemyShip.Move(i);
    auto [score, dir] = MaxNode(alpha, v, depth + 1);
    enemyShip.Move(i ^ 1);

    if (score < v) {
      v = score;
      bestDir = i;
    }
  }

  return {v, bestDir};
}
