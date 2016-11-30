#ifndef CHARACTER_HPP
#define CHARACTER_HPP
#include <map>
#include <string>
#include "Coord.hpp"

class Item;

const char PC_RENDER_C = '@';

class Character {
  protected:
    std::string name;
    char render_char;
    Coord coord;
    std::map<Item *, int> inventory;

  public:
    virtual char get_render_char() { return this->render_char; }
    void set_coord(int x, int y) { this->coord = Coord(x,y); }
    void set_coord(Coord coord) { this->coord = coord; }
    Coord get_coord() { return this->coord; }
    bool has(Item *item)  { return (this->inventory.find(item) != this->inventory.end()); }  
    void add_item(Item *);   
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
