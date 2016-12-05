/*************************************************************************
 * Program Filename:
 * Author: David Bacher-Hicks
 * Date:  3 December 2016
 * Description: A class declaration file for a Die class
 * Input: none
 * Output: none
 ************************************************************************/

#ifndef DIE_HPP
#define DIE_HPP

#include <stack>
#include <cstdlib>

class Die{
  private:
    int n_sides;
    int n_dice;
    int mod;

  public:
    Die(int n_sides, int n_dice = 1, int mod = 0);

    int roll();
    int max();
};

#endif
