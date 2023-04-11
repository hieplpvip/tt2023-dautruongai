#include "Minimax.h"
#include "Heuristic.h"
#include "Random.h"
#include "Store.h"
#include "Utility.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <chrono>

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
  // Get symmetric positions
  static std::vector<Position> sym[20][20];
  {
    bool horizontal = true;
    bool vertical = true;
    bool main_diagonal = (Store::M == Store::N);
    bool anti_diagonal = (Store::M == Store::N);

    REPL_ALL_CELL(i, j) {
      if (rootState.at[i][j] != rootState.at[Store::M - i - 1][j]) {
        horizontal = false;
      }
      if (rootState.at[i][j] != rootState.at[i][Store::N - j - 1]) {
        vertical = false;
      }

      if (Store::M == Store::N) {
        if (rootState.at[i][j] != rootState.at[j][i]) {
          main_diagonal = false;
        }
        if (rootState.at[i][j] != rootState.at[Store::M - i - 1][Store::N - j - 1]) {
          anti_diagonal = false;
        }
      }
    }

    REPL_ALL_CELL(i, j) {
      sym[i][j].clear();
      sym[i][j].emplace_back(i, j);
      if (horizontal) {
        sym[i][j].emplace_back(Store::M - i - 1, j);
      }
      if (vertical) {
        sym[i][j].emplace_back(i, Store::N - j - 1);
      }
      if (main_diagonal) {
        sym[i][j].emplace_back(j, i);
      }
      if (anti_diagonal) {
        sym[i][j].emplace_back(Store::M - i - 1, Store::N - j - 1);
      }
    }
  }

  auto candidates = Heuristic::GetCandidates(rootState, 10);
  auto [score, move] = Minimax::MaxStartNode(2 * -INF, 2 * INF, 0, candidates);
  auto& A = sym[move.x][move.y];
  move = A[Random::rand(A.size())];
  printFinalMove(move.x, move.y);
}

void makeMove() {
  auto [score, move] = Minimax::MaxNode(2 * -INF, 2 * INF, 0, rootState);
  printFinalMove(move.x, move.y);
}

int main() {
  // Seed our random number generator
  Random::seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

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
