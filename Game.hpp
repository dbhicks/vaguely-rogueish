#ifndef GAME_HPP
#define GAME_HPP
  
#include "Floor.hpp"
#include "Space.hpp"
#include "Character.hpp"
#include <fstream>
#include <sstream>

const std::string MAP_PATH_ROOT   = "gamedata/maps/";
const std::string ITEM_TBL_PATH   = "gamedata/items/items.tbl"; 
const std::string STARTING_MAP = "floor001";
const Coord STARTING_COORD = Coord(3,3);

class Game{
  private:
    std::vector<std::string> messages;    /* container of messages to print per round */
    std::map<std::string, Floor*> floors; /* game floors */
    std::map<std::string, Item*> items;   /* game item data */
    Player player;                        /* the player character */
    bool in_progress;                     /* whether the game is in progress */
    Floor *current_floor;                 /* pointer to the current floor */
    
  public:
    Game();
    ~Game();

    void load_floors();
    void load_items();
    void read_input(int input);
    void move_player(const direction &dir);
    void player_get_items();
    void move_mobs();
    std::string render();
    Coord coord_from_direction(const Coord &coord, const direction &dir);
    bool is_in_progress() { return this->in_progress; }


};

#endif
