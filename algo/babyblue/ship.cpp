#include "ship.h"

Ship::Ship() {}

void Ship::Read(ifstream &inp) {
  inp >> x >> y;
}

void Ship::ReadInfo(ifstream &inp) {
  inp >> g >> s;
}

void Ship::Move(int dir) {
  x += dx[dir];
  y += dy[dir];
}

void Ship::Place(int _x, int _y) {
  this->x = _x;
  this->y = _y;
}

void Ship::Print(ofstream &out) {
  out << x << " " << y << endl;
}
