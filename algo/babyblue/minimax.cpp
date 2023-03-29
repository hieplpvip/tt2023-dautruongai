#include "minimax.h"

MinimaxAgent::MinimaxAgent(){};

void MinimaxAgent::Input(ifstream &inp) {
  inp >> n >> m >> k;
  myShip.Read(inp);
  enemyShip.Read(inp);
  myShip.ReadInfo(inp);
  sea.Read(inp, n, m, myShip.s);
}

void MinimaxAgent::Load(ifstream &sav) {
  if (sav >> totalTurn) {
    sav >> lastMove;
    enemyShip.ReadInfo(sav);
    oldsea.Read(sav, n, m, 0);

    if (oldsea.at[enemyShip.x][enemyShip.y] == "S") {
      enemyShip.s = 1;
    } else if (oldsea.val[enemyShip.x][enemyShip.y] > 0) {
      enemyShip.g += oldsea.val[enemyShip.x][enemyShip.y];
    }
  }
}

void MinimaxAgent::Save(ofstream &sav) {
  sav << totalTurn << '\n';
  sav << lastMove << '\n';
  sav << enemyShip.g << ' ' << enemyShip.s << '\n';
  sea.Print(sav);
}

void MinimaxAgent::MakeMove(ofstream &out) {
  if (myShip.x == 0) {
    // First move
    auto [x, y] = Heuristic::FirstPlace(sea, myShip, enemyShip);
    myShip.Place(x, y);
    myShip.Print(out);
    return;
  }

  auto [score, dir] = MaxNode(-INF, INF, k, 0);
  myShip.Move(dir);
  myShip.Print(out);

  lastMove = dir;
}

// pair(score, dir)
pair<int, int> MinimaxAgent::MaxNode(int alpha, int beta, int turn, int depth) {
  // check terminal state
  // TODO: check terminal state when 2 ships collide + invalid grid
  // score = (gold+1)/2 * (bonus - (depth/2 + 1))
  // what if only one player die?
  // TODO: use heuristic search farther when not find any move
  if (turn == 0 || depth == MAX_DEPTH || (turn <= totalTurn / 2 && abs(myShip.g - enemyShip.g) > 20)) {
    return make_pair(myShip.score - enemyShip.score, -1);
  }

  // gen treasure
  if (turn == totalTurn / 2) {
    sea.val[n / 2 + 1][m / 2 + 1] = abs(myShip.g - enemyShip.g) * 3 / 4;
  }

  // select 2 best direction if depth < MAX_DEPTH
  // select 1 if depth >= MAX_DEPTH
  int v, r, sz, bestScore, bestDir;
  vector<pair<int, int>> cand;

  for (int i = 0; i < 4; ++i) {
    myShip.Move(i);
    v = Heuristic::Evaluate(sea, myShip, enemyShip);
    cand.push_back(make_pair(v, i));
    myShip.Move(i ^ 1);
  }
  sort(cand.rbegin(), cand.rend());

  // avoid going back
  if (depth == 0) {
    for (int j = 0; j < 4; ++j) {
      if (cand[j].second == (lastMove ^ 1)) {
        swap(cand[j], cand[3]);
        break;
      }
    }
  }

  bestScore = -INF;
  bestDir = -1;
  sz = depth < MAX_DEPTH ? 3 : 1;
  // sz = depth < MAX_DEPTH ? 2 : 1;

  for (int j = 0; j < sz && bestScore < beta; ++j) {
    int i = cand[j].second;

    // save state
    myShip.Move(i);

    if (!sea.isValid(myShip.x, myShip.y, myShip.s)) {
      myShip.Move(i ^ 1);
      continue;
    }

    if (sea.at[myShip.x][myShip.y] == "D" && myShip.s) {
      v = 0;
    } else {
      v = sea.val[myShip.x][myShip.y];
    }
    r = (v + 1) * (BONUS - (depth / 2 + 1));
    myShip.g += v;
    myShip.score += r;
    sea.val[myShip.x][myShip.y] -= v;

    auto [score, dir] = MinNode(v, beta, turn, depth + 1);

    // restore state
    sea.val[myShip.x][myShip.y] += v;
    myShip.g -= v;
    myShip.score -= r;
    myShip.Move(i ^ 1);

    if (score > bestScore) {
      bestScore = score;
      bestDir = i;
    }
  }

  // revert treasure
  if (turn == totalTurn / 2) {
    sea.val[n / 2 + 1][m / 2 + 1] = 0;
  }

  return {bestScore, bestDir};
}

pair<int, int> MinimaxAgent::MinNode(int alpha, int beta, int turn, int depth) {
  // select 2 best direction if depth < MAX_DEPTH
  // select 1 if depth >= MAX_DEPTH
  int v, r, sz, bestScore, bestDir;
  vector<pair<int, int>> cand;

  for (int i = 0; i < 4; ++i) {
    enemyShip.Move(i);
    v = Heuristic::Evaluate(sea, enemyShip, myShip);
    cand.push_back(make_pair(v, i));
    enemyShip.Move(i ^ 1);
  }
  sort(cand.rbegin(), cand.rend());

  bestScore = INF;
  bestDir = cand[0].second;
  sz = depth < MAX_DEPTH ? 3 : 1;
  // sz = depth < MAX_DEPTH ? 2 : 1;

  for (int j = 0; j < sz && bestScore > alpha; ++j) {
    int i = cand[j].second;

    // save state
    enemyShip.Move(i);

    if (!sea.isValid(enemyShip.x, enemyShip.y, enemyShip.s)) {
      enemyShip.Move(i ^ 1);
      continue;
    }

    if (sea.at[enemyShip.x][enemyShip.y] == "D" && enemyShip.s) {
      v = 0;
    } else {
      v = sea.val[enemyShip.x][enemyShip.y];
    }
    r = (v + 1) * (BONUS - (depth / 2 + 1));
    enemyShip.g += v;
    enemyShip.score += r;
    sea.val[enemyShip.x][enemyShip.y] -= v;

    auto [score, dir] = MaxNode(v, beta, turn, depth + 1);

    // restore state
    sea.val[enemyShip.x][enemyShip.y] += v;
    enemyShip.g -= v;
    enemyShip.score -= r;
    enemyShip.Move(i ^ 1);

    if (score < bestScore) {
      bestScore = score;
      bestDir = i;
    }
  }

  return {bestScore, bestDir};
}
