/*************************************************************************
 * Program Filename: Coord.cpp
 * Author: David Bacher-Hicks
 * Date: 3 December 2016
 * Description: A class definintion file for  a Coord class
 * Input: None
 * Output: None
 *************************************************************************/
#include "Coord.hpp"

/*************************************************************************
 * Function: operator<
 * Description: Less than comparator, necessary in order to use Coords as
 *              a map key. A coordinate is considered less than another if
 *              it appears above, or to the left of another at the same y val
 *              This definition allows the default map order to correspond
 *              to the render order.
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: Boolean - true if the coordinate is less than the righthand
 *          value
 ************************************************************************/
bool Coord::operator<(const Coord &right) const{
  return ((this->y_coord < right.y()) || 
          ((this->y_coord == right.y()) && (this->x_coord < right.x())));
}
