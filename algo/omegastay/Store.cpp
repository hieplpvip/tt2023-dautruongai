#include "Store.h"
#include <cassert>
#include <fstream>

namespace Store {
  int M, N, K, HALF_K;
  int currentTurn;

  int distNoShield[15][15][15][15];
  int distWithShield[15][15][15][15];

  int numLegalMovesNoShield[15][15];
  int numLegalMovesWithShield[15][15];

  bool isLegalMoveNoShield[15][15][NUM_MOVES];
  bool isLegalMoveWithShield[15][15][NUM_MOVES];

  std::vector<State> pastStates;

  void load() {
    std::ifstream fin("STATE.OUT", std::ios::binary);
    fin.read((char*)&M, sizeof(M));
    fin.read((char*)&N, sizeof(N));
    fin.read((char*)&K, sizeof(K));
    fin.read((char*)&HALF_K, sizeof(HALF_K));
    fin.read((char*)&currentTurn, sizeof(currentTurn));

    fin.read((char*)&distNoShield, sizeof(distNoShield));
    fin.read((char*)&distWithShield, sizeof(distWithShield));

    fin.read((char*)&numLegalMovesNoShield, sizeof(numLegalMovesNoShield));
    fin.read((char*)&numLegalMovesWithShield, sizeof(numLegalMovesWithShield));

    fin.read((char*)&isLegalMoveNoShield, sizeof(isLegalMoveNoShield));
    fin.read((char*)&isLegalMoveWithShield, sizeof(isLegalMoveWithShield));

    pastStates.resize(currentTurn);
    fin.read((char*)pastStates.data(), sizeof(State) * pastStates.size());
    fin.close();
  }

  void save() {
    assert(currentTurn == (int)pastStates.size());

    std::ofstream fout("STATE.OUT", std::ios::binary);
    fout.write((char*)&M, sizeof(M));
    fout.write((char*)&N, sizeof(N));
    fout.write((char*)&K, sizeof(K));
    fout.write((char*)&HALF_K, sizeof(HALF_K));
    fout.write((char*)&currentTurn, sizeof(currentTurn));

    fout.write((char*)&distNoShield, sizeof(distNoShield));
    fout.write((char*)&distWithShield, sizeof(distWithShield));

    fout.write((char*)&numLegalMovesNoShield, sizeof(numLegalMovesNoShield));
    fout.write((char*)&numLegalMovesWithShield, sizeof(numLegalMovesWithShield));

    fout.write((char*)&isLegalMoveNoShield, sizeof(isLegalMoveNoShield));
    fout.write((char*)&isLegalMoveWithShield, sizeof(isLegalMoveWithShield));

    fout.write((char*)pastStates.data(), sizeof(State) * pastStates.size());
    fout.close();
  }
}
