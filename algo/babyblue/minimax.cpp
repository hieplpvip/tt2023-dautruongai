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
  if (myShip.x != 0) {
    sav >> totalTurn;
    sav >> lastMove;
    enemyShip.ReadInfo(sav);
    oldsea.Read(sav, n, m, 0);

    if (oldsea.at[enemyShip.x][enemyShip.y] == "S") {
      enemyShip.s = 1;
    } else if (oldsea.val[enemyShip.x][enemyShip.y] > 0) {
      enemyShip.g += oldsea.val[enemyShip.x][enemyShip.y];
    }

    // FIXME: change rate
    myShip.score = sqr(myShip.g + 1);
    enemyShip.score = sqr(enemyShip.g + 1);
  } else {
    totalTurn = k;
    lastMove = -1;
    enemyShip.g = 0, enemyShip.s = 0;

    myShip.score = 0;
    enemyShip.score = 0;
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
    // auto [g, x, y] = Heuristic::BestPlace(sea, myShip, enemyShip, (n + m) / 3, k, totalTurn);
    auto [g, x, y] = Heuristic::BestPlace(sea, myShip, enemyShip, min(max(n, m) / 4, 4), k, totalTurn);
    myShip.Place(x, y);
    myShip.Print(out);
    return;
  }

  auto [score, dir] = MaxNode(-INF, INF, k, 0);
  if (dir != -1) myShip.Move(dir);
  myShip.Print(out);

  lastMove = dir;
}

// pair(score, dir)
pair<score_t, int> MinimaxAgent::MaxNode(score_t alpha, score_t beta, int turn, int depth) {
  _mx[depth] = myShip.x, _my[depth] = myShip.y, _mg[depth] = myShip.g;

  // check terminal state when 2 ships collide
  if ((depth > 0 && myShip.x == enemyShip.x && myShip.y == enemyShip.y) ||
      (depth > 0 && _mx[depth - 2] == enemyShip.x && _my[depth - 2] == enemyShip.y &&
       myShip.x == _ex[depth - 1] && myShip.y == _ey[depth - 1])) {
    score_t score = _mg[depth - 2] > _eg[depth - 1] ? INF : -INF;
    if (_mg[depth - 2] == _eg[depth - 1]) score = myShip.score - enemyShip.score;
    return make_pair(score, -1);
  }

  // what if only one player die?
  // TODO: check if enemy is eliminated

  // check terminal state
  if (turn == 0 || (turn <= totalTurn / 2 && abs(myShip.g - enemyShip.g) > 20)) {
    score_t score = myShip.g > enemyShip.g ? INF : (-INF + 5);
    score += myShip.score - enemyShip.score;
    return make_pair(score, -1);
  }

  // reach max depth, try searching farther using heuristic
  if (depth >= MAX_DEPTH) {
    // if can eat gold then do not search farther

    // auto [mg, mx, my] = Heuristic::BestPlace(sea, myShip, enemyShip, bval(2, 5, turn / 2), turn, totalTurn, false);
    // auto [eg, ex, ey] = Heuristic::BestPlace(sea, enemyShip, myShip, bval(2, 5, turn / 2), turn, totalTurn, false);

    auto [mg, mx, my] = Heuristic::HighestHeat(sea, myShip, enemyShip, bval(2, 3, turn / 2), turn, totalTurn);
    auto [eg, ex, ey] = Heuristic::HighestHeat(sea, enemyShip, myShip, bval(2, 3, turn / 2), turn, totalTurn);

    score_t myShipScore = myShip.score + mg;
    score_t enemyShipScore = enemyShip.score + eg;
    return make_pair(myShipScore - enemyShipScore, -1);
  }

  // gen treasure
  if (turn == totalTurn / 2) {
    sea.val[n / 2 + 1][m / 2 + 1] = abs(myShip.g - enemyShip.g) * 3 / 4;
  }

  int sz, bestDir;
  score_t v, r, bestScore;
  vector<pair<score_t, int>> cand;

  // for (int i = 0; i < 4; ++i) {
  //   myShip.Move(i);
  //   v = Heuristic::Evaluate(sea, myShip, enemyShip, myShip.x, myShip.y, bval(2, 5, turn / 2), k, totalTurn);
  //   cand.push_back(make_pair(v, i));
  //   myShip.Move(i ^ 1);
  // }

  auto [g, x, y] = Heuristic::HighestHeat(sea, myShip, enemyShip, bval(2, 3, turn / 2), turn, totalTurn);
  for (int i = 0; i < 4; ++i) {
    myShip.Move(i);
    cand.emplace_back(dist(myShip.x, myShip.y, x, y), i);
    myShip.Move(i ^ 1);
  }

  sort(cand.begin(), cand.end());

  // avoid going back
  if (depth == 0) {
    for (int j = 0; j < 4; ++j) {
      if (cand[j].second == (lastMove ^ 1)) {
        swap(cand[j], cand[3]);
        break;
      }
    }
  } else {
    for (int j = 0; j < 4; ++j) {
      int i = cand[j].second;
      myShip.Move(i);
      if (myShip.x == _mx[depth - 2] && myShip.y == _my[depth - 2])
        swap(cand[j], cand[3]);
      myShip.Move(i ^ 1);
    }
  }

  bestScore = -INF;
  bestDir = -1;
  sz = depth < MAX_DEPTH ? 3 : 1;

  for (int j = 0; j < sz && bestScore < beta; ++j) {
    int i = cand[j].second;

    // save state
    myShip.Move(i);

    if (!sea.isValid(myShip.x, myShip.y, myShip.s)) {
      myShip.Move(i ^ 1);
      continue;
    }

    if ((sea.at[myShip.x][myShip.y] == "D" && myShip.s) ||  // danger grid
        (sea.at[myShip.x][myShip.y] == "S" && myShip.s)) {  // shield grid
      v = 0;
    } else {
      v = sea.val[myShip.x][myShip.y];
    }

    r = sqr(v + 1) / sqrt(depth / 2 + 1);
    if (v > 0 && depth == 0) r += BONUS;

    if (sea.at[myShip.x][myShip.y] != "S") myShip.g += v;
    myShip.score += r;
    if (sea.at[myShip.x][myShip.y] != "D")
      sea.val[myShip.x][myShip.y] -= v;

    auto [score, dir] = MinNode(max(bestScore, alpha), beta, turn, depth + 1);

    // restore state
    if (sea.at[myShip.x][myShip.y] != "D")
      sea.val[myShip.x][myShip.y] += v;
    if (sea.at[myShip.x][myShip.y] != "S") myShip.g -= v;
    myShip.score -= r;
    myShip.Move(i ^ 1);

    if (score > bestScore) {
      bestScore = score;
      bestDir = i;
    }

    if (bestDir == -1) sz = 4;
  }

  // revert treasure
  if (turn == totalTurn / 2) {
    sea.val[n / 2 + 1][m / 2 + 1] = 0;
  }

  return {bestScore, bestDir};
}

pair<score_t, int> MinimaxAgent::MinNode(score_t alpha, score_t beta, int turn, int depth) {
  _ex[depth] = enemyShip.x, _ey[depth] = enemyShip.y, _eg[depth] = enemyShip.g;

  int sz, bestDir;
  score_t v, r, bestScore;
  vector<pair<score_t, int>> cand;

  // for (int i = 0; i < 4; ++i) {
  //   enemyShip.Move(i);
  //   v = Heuristic::Evaluate(sea, enemyShip, myShip, enemyShip.x, enemyShip.y, bval(2, 5, turn / 2), k, totalTurn);
  //   cand.push_back(make_pair(v, i));
  //   enemyShip.Move(i ^ 1);
  // }

  auto [g, x, y] = Heuristic::HighestHeat(sea, enemyShip, myShip, bval(2, 3, turn / 2), turn, totalTurn);
  for (int i = 0; i < 4; ++i) {
    enemyShip.Move(i);
    cand.emplace_back(dist(enemyShip.x, enemyShip.y, x, y), i);
    enemyShip.Move(i ^ 1);
  }

  sort(cand.rbegin(), cand.rend());

  if (depth > 1) {
    for (int j = 0; j < 4; ++j) {
      int i = cand[j].second;
      enemyShip.Move(i);
      if (enemyShip.x == _ex[depth - 2] && enemyShip.y == _ey[depth - 2])
        swap(cand[j], cand[3]);
      enemyShip.Move(i ^ 1);
    }
  }

  bestScore = INF;
  bestDir = -1;
  sz = depth < MAX_DEPTH ? 3 : 1;

  for (int j = 0; j < sz && bestScore > alpha; ++j) {
    int i = cand[j].second;

    // save state
    enemyShip.Move(i);

    if (!sea.isValid(enemyShip.x, enemyShip.y, enemyShip.s)) {
      enemyShip.Move(i ^ 1);
      continue;
    }

    if ((sea.at[enemyShip.x][enemyShip.y] == "D" && enemyShip.s) ||  // danger grid
        (sea.at[enemyShip.x][enemyShip.y] == "S" && enemyShip.s)) {  // shield grid
      v = 0;
    } else {
      v = sea.val[enemyShip.x][enemyShip.y];
    }

    r = sqr(v + 1) / sqrt(depth / 2 + 1);
    if (v > 0 && depth == 1) r += BONUS;

    if (sea.at[enemyShip.x][enemyShip.y] != "S") enemyShip.g += v;
    enemyShip.score += r;
    if (sea.at[enemyShip.x][enemyShip.y] != "D")
      sea.val[enemyShip.x][enemyShip.y] -= v;

    auto [score, dir] = MaxNode(alpha, min(beta, bestScore), turn - 1, depth + 1);

    // restore state
    if (sea.at[enemyShip.x][enemyShip.y] != "D")
      sea.val[enemyShip.x][enemyShip.y] += v;
    if (sea.at[enemyShip.x][enemyShip.y] != "S") enemyShip.g -= v;
    enemyShip.score -= r;
    enemyShip.Move(i ^ 1);

    if (score < bestScore) {
      bestScore = score;
      bestDir = i;
    }
  }

  return {bestScore, bestDir};
}
