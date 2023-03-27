#include "ship.h"

Ship::Ship() {}

void Ship::Read(ifstream &inp) {
  inp >> x >> y;
}

void Ship::ReadInfo(ifstream &inp) {
  // inp >> g >> s;
  inp >> s >> g;
}

void Ship::Move(int dir) {
  x += dx[dir];
  y += dy[dir];
}

void Ship::Place(int x, int y) {
  this->x = x;
  this->y = y;
}

void Ship::Print(ofstream &out) {
  out << x << " " << y << endl;
}
