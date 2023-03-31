#include <bits/stdc++.h>
#include "babyblue.h"
#include "constant.h"
#include "estimate-shield-val.h"
#include "heisenberg.h"
#include "maplist.h"
using namespace std;

int main() {
  int algo = -1;

  int m, n, k;
  int x1, y1, x2, y2;
  ifstream inp("MAP.INP");
  inp >> m >> n >> k;
  inp >> x1 >> y1 >> x2 >> y2;

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

    for (int k = 0; k < MAPLIST_LEN; ++k) {
      if (m == MAP_M[k] && m == MAP_N[k] && A == MAPS[k]) {
        algo = ALGOS[k];
        break;
      }
    }

    if (algo == -1) {
      algo = ALGO_BABYBLUE;
    }

    cerr << "algo = " << algo << endl;
  } else {
    // 2nd turn and later. Load algo from STATE.OUT
    ifstream state("STATE.OUT");
    state >> algo;
    state.close();
  }
  inp.close();

  switch (algo) {
    case ALGO_HEISENBERG:
      cerr << "Running Heisenberg\n";
      Heisenberg::main();
      break;

    case ALGO_ESTIMATE_SHIELD_VAL:
      cerr << "Running estimate_shield_val\n";
      estimate_shield_val::main();
      break;

    default:
      cerr << "Running BabyBlue\n";
      BabyBlue::main();
      break;
  }

  return 0;
}
