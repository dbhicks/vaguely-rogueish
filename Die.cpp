#include "Die.hpp"

Die::Die(int n_sides, int n_dice, int mod)
{
  this->n_sides = n_sides;
  this->n_dice = n_dice;
  this->mod = mod;
}

int Die::roll()
{
  int roll_sum = 0;

  for( int i = 0; i < this->n_dice; i++ ){
    roll_sum += (rand() % this->n_sides + 1);
  }

  return roll_sum + mod;  
}

int Die::max()
{
  return this->n_sides * this->n_dice + this->mod;
}
