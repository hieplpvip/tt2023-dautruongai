#include <bits/stdc++.h>
#include "Constants.h"
#include "Engine.h"
#include "Store.h"
using namespace std;

/*
 * Read input from MAP.INP
 *
 * @return true if first turn
 */
bool readInput() {
  // Open file for reading
  ifstream fin("MAP.INP");

  // Reading map size
  int m, n, k;
  fin >> m >> n >> k;

  // Read in positions
  fin >> rootState.pos[0].x >> rootState.pos[0].y;
  fin >> rootState.pos[1].x >> rootState.pos[1].y;

  // Read in our gold/shield
  fin >> rootState.gold[0] >> rootState.hasShield[0];

  // Read in the map
  string s;
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      fin >> s;
      if (s[0] == 'D') {
        rootState.cell[i][j] = DANGER_CELL;
      } else if (s[0] == 'S') {
        rootState.cell[i][j] = SHIELD_CELL;
        shieldPos.emplace_back(i, j);
      } else {
        rootState.cell[i][j] = stoi(s);
      }
    }
  }

  // Close file
  fin.close();

  bool firstTurn = rootState.pos[0].x == 0 && rootState.pos[0].y == 0 && rootState.pos[1].x == 0 && rootState.pos[1].y == 0;
  if (firstTurn) {
    // For first turn, we need to save m, n, k in Store
    Store::M = m;
    Store::N = n;
    Store::K = k;
  } else {
    // Convert to 0-index positions
    --rootState.pos[0].x;
    --rootState.pos[0].y;
    --rootState.pos[1].x;
    --rootState.pos[1].y;
  }

  return firstTurn;
}

int main() {
  bool firstTurn = readInput();
  if (firstTurn) {
    handleFirstTurn();
  } else {
    handleOtherTurns();
  }
  return 0;
}