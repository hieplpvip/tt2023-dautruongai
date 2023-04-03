#include "Store.h"
#include <cassert>
#include <fstream>

namespace Store {
  int M, N, K;
  int currentTurn;
  int distNoShield[15][15][15][15];
  int distWithShield[15][15][15][15];
  std::vector<State> pastStates;

  void load() {
    std::ifstream fin("STATE.OUT", std::ios::binary);
    fin.read((char*)&M, sizeof(M));
    fin.read((char*)&N, sizeof(N));
    fin.read((char*)&K, sizeof(K));
    fin.read((char*)&currentTurn, sizeof(currentTurn));
    fin.read((char*)&distNoShield, sizeof(distNoShield));
    fin.read((char*)&distWithShield, sizeof(distWithShield));
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
    fout.write((char*)&currentTurn, sizeof(currentTurn));
    fout.write((char*)&distNoShield, sizeof(distNoShield));
    fout.write((char*)&distWithShield, sizeof(distWithShield));
    fout.write((char*)pastStates.data(), sizeof(State) * pastStates.size());
    fout.close();
  }
}
