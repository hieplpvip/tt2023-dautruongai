#include "main.h"
#include "minimax.h"

int main()
{
  MinimaxAgent agent;

  ifstream inp("MAP.INP");
  ifstream loads("STATE.OUT");
  agent.Input(inp);
  agent.Load(loads);
  inp.close();
  loads.close();

  ofstream out("MOVE.OUT");
  ofstream sav("STATE.OUT");
  agent.MakeMove(out);
  agent.Save(sav);
  out.close();
  sav.close();
}
