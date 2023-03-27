#include "map.h"

Map::Map() {}

void Map::Read(ifstream &inp, int n, int m)
{
  this->n = n;
  this->m = m;
  for (int i = 1; i <= n; ++i)
    for (int j = 1; j <= m; ++j)
    {
      inp >> at[i][j];

      if (at[i][j] == "D")
        val[i][j] = -INF;
      else if (at[i][j] == "S")
        val[i][j] = SHIELD_SCORE;
      else
        val[i][j] = stoi(at[i][j]);
    }
}

bool Map::isValid(int x, int y, int s)
{
  return x >= 1 && x <= n && y >= 1 && y <= m && (s || at[x][y] != "D");
}
