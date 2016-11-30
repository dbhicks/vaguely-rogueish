#ifndef FLOOR_HPP
#define FLOOR_HPP

#include <map>
#include <vector>
#include <string>
#include <fstream>

#include "Coord.hpp"
#include "Space.hpp"

class Character;

class Floor{
  private:
    std::map<Coord, Space *> spaces;
    
  public:
    ~Floor();
    Space *get_space(int x, int y) { return this->spaces[Coord(x,y)]; }
    void add_space(Space *space, int x, int y) 
      { this->spaces.insert(std::pair<Coord, Space *>(Coord(x,y), space)); }

    void load_floor(std::string path);
    std::string render_floor();
    Space * interpret_space(char space_char, Coord coord);

    bool add_char(Character *, const Coord &coord);
    bool move_char(const Coord &from, const Coord &to);
};

#endif
