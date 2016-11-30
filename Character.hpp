#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <string>
#include "Coord.hpp"

const char PC_RENDER_C = '@';

class Character {
  protected:
    std::string name;
    char render_char;
    Coord coord;
    std::map<std::string, int> inventory;

  public:
    virtual char get_render_char() { return this->render_char; }
    void set_coord(int x, int y) { this->coord = Coord(x,y); }
    Coord get_coord() { return this->coord; }
    bool has(std::string itemID)  { return (this->inventory.find(itemID) != this->inventory.end()); }    
};

class Player : public Character {
  private:
  public:    
    virtual char get_render_char() { return PC_RENDER_C; }
};

class Mob : public Character {
  public:
  private:    
};

#endif
