/*************************************************************************
 * Program Filename: Die.cpp
 * Author: David Bacher-Hicks
 * Date: 3 December 2016
 * Description: A class definintion file for a die class
 * Input: none
 * Output: none
 ************************************************************************/

#include "Die.hpp"

/*************************************************************************
 * Function: Die
 * Description: constructor
 * Parameters: n_dice   - the number of dice
 *             n_sides  - the number of sides
 *             mod      - the die roll modifier (+/-)
 *
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: none
 ************************************************************************/
Die::Die(int n_dice, int n_sides, int mod)
{
  this->n_sides = n_sides;
  this->n_dice = n_dice;
  this->mod = mod;
}


/*************************************************************************
 * Function: roll
 * Description: rolls the die and returns the result
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions:none 
 * Returns: int - die roll result
 ************************************************************************/
int Die::roll()
{
  int roll_sum = 0;

  for( int i = 0; i < this->n_dice; i++ ){
    roll_sum += (rand() % this->n_sides + 1);
  }

  return roll_sum + mod;  
}


/*************************************************************************
 * Function: max
 * Description: returns the maximum value a die may return
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: int - maximum die roll value
 ************************************************************************/
int Die::max()
{
  return this->n_sides * this->n_dice + this->mod;
}
