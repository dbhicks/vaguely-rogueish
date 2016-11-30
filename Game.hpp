#ifndef GAME_HPP
#define GAME_HPP
  
#include "Floor.hpp"
#include "Space.hpp"
#include "Character.hpp"
#include <fstream>
#include <sstream>

const std::string MAP_PATH_ROOT = "gamedata/maps/";
const std::string STARTING_MAP = "floor001";
const Coord STARTING_COORD = Coord(3,3);

class Game{
  private:
    std::vector<std::string> messages;
    std::map<std::string, Floor*> floors;
    Player player;
    bool in_progress;

    Floor *current_floor;
    
  public:
    Game();
    ~Game();

    void load_floors();
    void read_input(int input);
    void move_player(const direction &dir);
    void move_mobs();
    std::string render();
    Coord coord_from_direction(const Coord &coord, const direction &dir);
    bool is_in_progress() { return this->in_progress; }
};

#endif
