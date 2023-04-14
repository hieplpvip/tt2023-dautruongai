#include "AlgoList.h"
#include "BabyBlue.h"
#include "Constants.h"
#include "Engine.h"
#include "Minimax.h"
#include "Store.h"
#include "Random.h"
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

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

void chooseAlgo() {
  std::ifstream inp("MAP.INP");

  // Ignore first 3 lines
  {
    int tmp;
    inp >> tmp >> tmp >> tmp;
    inp >> tmp >> tmp >> tmp >> tmp;
    inp >> tmp >> tmp;
  }

  // Read map
  std::vector<char> A(Store::M * Store::N);
  for (char& c : A) {
    inp >> c;
  }
  inp.close();

  // Default to OmegaStay
  Store::algo = ALGO_OMEGASTAY;

  // Find in map list
  for (int i = 0; i < AlgoList::MAPLIST_LEN; ++i) {
    if (Store::M == AlgoList::MAP_M[i] && Store::N == AlgoList::MAP_N[i] && A == AlgoList::MAPS[i]) {
      Store::algo = AlgoList::ALGOS[i];
      break;
    }
  }
}

int main() {
  // Seed our random number generator
  Random::seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

  bool firstTurn = readInput();
  if (firstTurn) {
    Store::init();
    chooseAlgo();
    Store::save();
  } else {
    Store::load();
    Store::update();
    Store::save();
  }

  switch (Store::algo) {
    case ALGO_BABYBLUE:
#ifdef ENABLE_LOGGING
      std::cerr << "Running BabyBlue" << std::endl;
#endif
      BabyBlue::run();
      break;

    case ALGO_OHMYBLUE:
#ifdef ENABLE_LOGGING
      std::cerr << "Running oh-my-blue" << std::endl;
#endif
      Minimax::run();
      break;

    case ALGO_OMEGASTAY:
#ifdef ENABLE_LOGGING
      std::cerr << "Running OmegaStay" << std::endl;
#endif
      Engine::run();
      break;
  }

#ifdef PRINT_MESSAGE
  std::cerr << "From Seno with love <3" << std::endl;
#endif

  return 0;
}
