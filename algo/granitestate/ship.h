#ifndef SHIP_H
#define SHIP_H

#include "babyblue.h"

class Ship {
public:
  Ship();

  void Read(ifstream &inp);
  void ReadInfo(ifstream &inp);
  void Print(ofstream &out);

  void Move(int dir);
  void Place(int x, int y);

  int x, y, g, s;
  score_t score;
};

#endif  // SHIP_H
