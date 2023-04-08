#include "Store.h"
#include <cassert>
#include <fstream>

namespace Store {
  int M, N, K, HALF_K;
  int currentTurn;

  int dist[2][15][15][15][15];
  int numLegalMoves[2][15][15];
  bool isLegalMove[2][15][15][NUM_MOVES];

  std::vector<State> pastStates;

  void load() {
    std::ifstream fin("STATE.OUT", std::ios::binary);
    fin.read((char*)&M, sizeof(M));
    fin.read((char*)&N, sizeof(N));
    fin.read((char*)&K, sizeof(K));
    fin.read((char*)&HALF_K, sizeof(HALF_K));
    fin.read((char*)&currentTurn, sizeof(currentTurn));
    fin.read((char*)&dist, sizeof(dist));
    fin.read((char*)&numLegalMoves, sizeof(numLegalMoves));
    fin.read((char*)&isLegalMove, sizeof(isLegalMove));
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
    fout.write((char*)&dist, sizeof(dist));
    fout.write((char*)&numLegalMoves, sizeof(numLegalMoves));
    fout.write((char*)&isLegalMove, sizeof(isLegalMove));
    fout.write((char*)pastStates.data(), sizeof(State) * pastStates.size());
    fout.close();
  }
}
