#include <bits/stdc++.h>
#include "babyblue.h"
#include "constant.h"
#include "estimate-shield-val.h"
#include "heisenberg.h"
#include "maplist.h"
using namespace std;

int main() {
  int algo = -1;

  int m, n, k, gold, shield;
  int x1, y1, x2, y2;
  ifstream inp("MAP.INP");
  inp >> m >> n >> k;
  inp >> x1 >> y1 >> x2 >> y2;
  inp >> gold >> shield;

  if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0) {
    // First turn. Select algorithm.
    vector<char> A;
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        char a;
        inp >> a;
        A.push_back(a);
      }
    }

    for (int i = 0; i < MAPLIST_LEN; ++i) {
      if (m == MAP_M[i] && n == MAP_N[i] && A == MAPS[i]) {
        algo = ALGOS[i];
        break;
      }
    }

    if (algo == -1) {
      algo = ALGO_BABYBLUE;
    }
  } else {
    // 2nd turn and later. Load algo from STATE.OUT
    ifstream state("STATE.OUT");
    state >> algo;
    state.close();
  }
  inp.close();

  // cerr << "algo = " << algo << endl;
  switch (algo) {
    case ALGO_HEISENBERG:
      // cerr << "Running Heisenberg\n";
      return Heisenberg::main();

    case ALGO_ESTIMATE_SHIELD_VAL:
      // cerr << "Running estimate_shield_val\n";
      return estimate_shield_val::main();

    default:
      // cerr << "Running BabyBlue\n";
      return BabyBlue::main();
  }
}
