#include "Minimax.h"
#include "State.h"
#include "Store.h"
#include <cassert>
#include <iostream>
#include <fstream>

/*
 * Read input from MAP.INP
 *
 * @return true if first turn
 */
bool readInput() {
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

  auto [score, move] = minimax.MaxStartNode(2 * -INF, 2 * INF, 0);

  // Write result to file
  std::ofstream fout("MOVE.OUT");
  fout << move.x + 1 << " " << move.y + 1;
  fout.close();
}

void makeMove() {
  MiniMaxAlgorithm minimax;

  auto [score, move] = minimax.MaxNode(2 * -INF, 2 * INF, 0, rootState);

  // Write result to file
  std::ofstream fout("MOVE.OUT");
  fout << move.x + 1 << " " << move.y + 1 << '\n';
  fout.close();
}

int main() {
  bool firstTurn = readInput();
  if (firstTurn) {
    Store::init();
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
