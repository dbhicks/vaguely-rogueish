/*************************************************************************
 * Program Filename: Floor.hpp
 * Author: David Bacher-Hicks
 * Date: 3 December 2016
 * Description: A class declaration file for a Floor class.
 * Input:   none
 * Output:  none
 ************************************************************************/
#ifndef FLOOR_HPP
#define FLOOR_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <fstream>

#include "Coord.hpp"
#include "Space.hpp"

class Character;

class Floor{
  private:
    std::map<Coord, Space *> spaces;
    std::set<Character *> mob_list;
    
  public:
    ~Floor();
    Space *get_space(int x, int y) { return this->spaces[Coord(x,y)]; }
    Space *get_space(Coord coord) { return this->spaces[coord]; }
    void add_space(Space *space, int x, int y) 
      { this->spaces.insert(std::pair<Coord, Space *>(Coord(x,y), space)); }

    void load_floor(std::string path);
    std::string render_floor();
    Space * interpret_space(char space_char, Coord coord);
    bool add_char(Character *, const Coord &coord);
    bool move_char(const Coord &from, const Coord &to);
    void list_mob(Character *mob) { this->mob_list.insert(mob); } 
    void unlist_mob(Character *mob) { this->mob_list.erase(this->mob_list.find(mob)); }
    std::set<Character *> *get_mob_list() { return &(this->mob_list); }
    std::map<Coord, Space *> *get_spaces() { return &(this->spaces); }
};

#endif
