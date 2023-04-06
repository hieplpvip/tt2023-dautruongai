#include <iostream>
#include <fstream>
#include <assert.h>

#include "state.h"
#include "store.h"
#include "minimax.h"

bool input() {
  // Open file for reading
  std::ifstream fin("MAP.INP");

  // Reading map size
  int m, n, k;
  fin >> m >> n >> k;

  rootState.turnLeft = k;
  assert(rootState.turnLeft > 0);

  // Read in positions
  fin >> rootState.pos[0].x >> rootState.pos[0].y;
  fin >> rootState.pos[1].x >> rootState.pos[1].y;

  // Convert to 0-index positions
  --rootState.pos[0].x;
  --rootState.pos[0].y;
  --rootState.pos[1].x;
  --rootState.pos[1].y;

  // Read in our gold/shield
  fin >> rootState.gold[0] >> rootState.hasShield[0];

  // Read in the map
  std::string s;
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      fin >> s;
      if (s[0] == 'D') {
        rootState.at[i][j] = DANGER_CELL;
      } else if (s[0] == 'S') {
        rootState.at[i][j] = SHIELD_CELL;
      } else {
        rootState.at[i][j] = stoi(s);
      }
    }
  }

  // Close file
  fin.close();

  bool firstTurn =
      rootState.pos[0].x == -1 &&
      rootState.pos[0].y == -1 &&
      rootState.pos[1].x == -1 &&
      rootState.pos[1].y == -1;

  if (firstTurn) {
    // For first turn, we need to save m, n, k in Store
    Store::M = m;
    Store::N = n;
    Store::K = k;
    Store::HALF_K = k / 2;
  }

  return firstTurn;
}

void makeFirstMove() {
  MiniMaxAlgorithm minimax;

  score_t bestScore = -INF, secondBestScore = -INF;
  Position bestMove, secondBestMove;

  // FIXME: Find candidate positions for first move and remove turnLeft limit
  // limit turnLeft to 10
  rootState.turnLeft = std::min(rootState.turnLeft, 10);

  // FIXME: Do we need to evaluate opponent's first move?
  REPL_ALL_CELL(x, y) {
    if (rootState.at[x][y] != EMPTY_CELL) {
      continue;
    }

    score_t minScore = INF;

    REPL_ALL_CELL(u, v) {
      if (rootState.at[u][v] != EMPTY_CELL || (x == u && y == v)) {
        continue;
      }

      rootState.pos[0] = rootState.lastPos[0] = Position(x, y);
      rootState.pos[1] = rootState.lastPos[1] = Position(u, v);

      auto [score, _] = minimax.MaxNode(-INF, INF, MAX_DEPTH, rootState);  // start with MAX_DEPTH to use magnet

      if (score < minScore) {
        minScore = score;
      }
    }

    if (minScore > bestScore) {
      secondBestScore = bestScore;
      secondBestMove = bestMove;
      bestScore = minScore;
      bestMove = Position(x, y);
    } else if (minScore > secondBestScore) {
      secondBestScore = minScore;
      secondBestMove = Position(x, y);
    }
  }

  // FIXME: Use random(2) if needed

  // Write result to file
  std::ofstream fout("MOVE.OUT");
  fout << bestMove.x + 1 << " " << bestMove.y + 1;
  fout.close();
}

void makeMove() {
  MiniMaxAlgorithm minimax;

  auto [score, move] = minimax.MaxNode(-INF, INF, 0, rootState);

  // Write result to file
  std::ofstream fout("MOVE.OUT");
  fout << move.x + 1 << " " << move.y + 1 << '\n';
  fout.close();
}

int main() {
  bool firstTurn = input();
  if (firstTurn) {
    Store::init();
    Store::update();
    Store::save();
    makeFirstMove();
  } else {
    Store::load();
    Store::update();
    Store::save();
    makeMove();
  }
  return 0;
}