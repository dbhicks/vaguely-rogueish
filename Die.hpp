#ifndef DIE_HPP
#define DIE_HPP

#include <stack>
#include <cstdlib>

class ERR_INVALID_DIESTRING {};

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
