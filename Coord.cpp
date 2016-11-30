#include "Coord.hpp"

bool Coord::operator<(const Coord &right) const{
  return ((this->y_coord < right.y()) || 
          ((this->y_coord == right.y()) && (this->x_coord < right.x())));
}
