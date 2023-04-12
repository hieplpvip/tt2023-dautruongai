#ifndef MAP_H
#define MAP_H

#include "main.h"

class Map {
public:
  Map();

  void Read(ifstream &inp, int _n, int _m);
  void Print(ofstream &out);

  bool isValid(int x, int y, int s);

  int n, m;
  string at[MAXN][MAXN];
  int val[MAXN][MAXN];
};

#endif  // MAP_H
