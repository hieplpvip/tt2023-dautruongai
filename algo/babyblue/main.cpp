#include "main.h"
#include "minimax.h"

int main() {
  MinimaxAgent agent;

  ifstream inp("MAP.INP");
  agent.Input(inp);
  inp.close();

  ifstream loads("STATE.OUT");
  agent.Load(loads);
  loads.close();

  ofstream out("MOVE.OUT");
  agent.MakeMove(out);
  out.close();

  ofstream sav("STATE.OUT");
  agent.Save(sav);
  sav.close();
}
