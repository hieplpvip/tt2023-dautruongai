#include "main.h"
#include "minimax.h"

int main() {
  ifstream inp("MAP.INP");
  ofstream out("MOVE.OUT");
  ofstream sav("STATE.OUT");

  MinimaxAgent agent;
  agent.Input(inp);
  agent.MakeMove(out);

  inp.close();
  sav.close();
  out.close();
}
