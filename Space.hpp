#ifndef SPACE_HPP
#define SPACE_HPP

#include <map>
#include <vector>
#include <string>
#include "Item.hpp"
#include "Coord.hpp"

const char OPEN_DOOR_C    = '/';
const char CLOSED_DOOR_C  = '[';
const char EMPTY_SPACE_C  = '.';
const char ITEM_SPACE_C   = '*';
const char WALL_C         = '#';
const char UP_STAIR_C     = '^';
const char DOWN_STAIR_C   = 'v';
const char HIDDEN_DOOR_C  = '%'; 

enum direction {UP, RIGHT, DOWN, LEFT};

class Floor;
class Character;

class Space{
  protected:
    std::vector<Item*> items;
    Character *present_character;
    Coord coord;
    char render_char;
    std::map<direction, Space *> linked_spaces;

  public:
    /* Constructors, Destructors, Pure Virtuals */
    Space(const Coord &coord);
    Space(int x, int y) : Space(Coord(x,y)) {}
    virtual ~Space() {}; 
    virtual bool passable() = 0;
    virtual bool is_locked() { return false; };

    /* Character methods */
    Character *get_character() { return this->present_character; }
    bool add_character(Character *character);
    bool delete_character();
    Character *remove_character();

    /* Item methods */
    std::vector<Item*> *get_items() { return &items; }
    virtual bool add_item(Item *item) { return false; }
    virtual Item *remove_item(std::string itemID) { return NULL; }

    /* render character getters/setters */
    void set_render_char(char c) { this->render_char = c; }
    virtual char get_render_char() const;

    /* Coordinate getters */
    Coord get_coord() { return this->coord; }
    int x() { return this->coord.x(); }
    int y() { return this->coord.y(); }
};

class OpenSpace : public Space {
  public:
    OpenSpace(const Coord &coord) : Space(coord) { this->render_char = EMPTY_SPACE_C; }    
    OpenSpace(int x, int y) : Space(x, y) { this->render_char = EMPTY_SPACE_C; }
    virtual bool passable();
    virtual char get_render_char() const;
    virtual bool add_item(Item *item) { this->items.push_back(item); return true; }
    virtual Item * remove_item(std::string itemID);
};

class Wall : public Space {
  protected:
  public:
    Wall(const Coord &coord) : Space(coord) { render_char = WALL_C; }
    Wall(int x, int y) : Space(x, y) { render_char = WALL_C; }
    virtual bool passable() { return false; }
};

class Door : public Space {
  protected:
    bool is_open;
    std::string keyID;
        
  public:
    Door(const Coord &coord, std::string keyID = "") : 
      Space(coord) { this->is_open = false; this->keyID = keyID; }

    Door(int x, int y, std::string keyID = "") : 
      Space(x, y) { this->is_open = false; this->keyID = keyID; }

    virtual bool passable() { return this->is_open; }
    virtual char get_render_char() const;
    virtual bool is_locked() { return !(this->keyID == ""); }
    std::string key_str() { return this->keyID; }
    void set_key(std::string keyID) { this->keyID = keyID; }
    bool open();
    bool close();
};

class SecretDoor : public Space {
  protected:
    bool is_open;
        
  public:
    SecretDoor(const Coord &coord) : 
      Space(coord) { this->is_open = false; }

    SecretDoor(int x, int y) : 
      Space(x, y) { this->is_open = false; }

    virtual bool passable() { return this->is_open; }
    virtual char get_render_char() const; 
    bool open(); 
};



class Stair : public Space {
  protected:
    Coord linked_coord;
    std::string linked_floor_ID;

  public:
    Stair(const Coord &coord, char c) : 
      Space(coord) { this->render_char = c; }
    Stair(int x, int y, char c) : 
      Space(x, y) { this->render_char = c; }

    void set_linked_coord(Coord coord) { this->linked_coord = coord; }
    void set_linked_floor_ID( std::string id ) { this->linked_floor_ID = id; }
    std::string get_linked_floor_ID () { return this->linked_floor_ID; }
    Coord get_linked_coord () { return this->linked_coord; }

    virtual bool passable() { return true; }
};

class UpStair : public Stair {
  public:   
    UpStair(const Coord &coord) : 
      Stair(coord, UP_STAIR_C) {}

    UpStair(int x, int y, Space *linked_stair = NULL, Floor *linked_floor = NULL) : 
      Stair(x, y, UP_STAIR_C) {}
};

class DownStair : public Stair {
  public:
    DownStair(const Coord &coord) : 
      Stair(coord, DOWN_STAIR_C) {}

    DownStair(int x, int y) : 
      Stair(x, y, DOWN_STAIR_C) {}
};

#endif
