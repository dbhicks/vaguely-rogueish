/*************************************************************************
 * Program File: Game.hpp
 * Author: David Bacher-Hicks
 * Date: 3 December 2016
 * Description: A class declaration file for a game object that handles
 *              operating a roguelike game.
 * Input: Reads game data from gamedata files
 *        standard in
 * Output: standard out
 ************************************************************************/

#ifndef GAME_HPP
#define GAME_HPP
  
#include "Floor.hpp"
#include "Space.hpp"
#include "Character.hpp"
#include <fstream>
#include <sstream>
#include <set>

/* Gamedata paths */
const std::string MAP_PATH_ROOT   = "gamedata/maps/";
const std::string ITEM_TBL_PATH   = "gamedata/items/items.tbl";
const std::string WPN_TBL_PATH    = "gamedata/items/weapons.tbl";
const std::string AMR_TBL_PATH    = "gamedata/items/armor.tbl";
const std::string LOGFILE_PATH    = "gamedata/log";
const std::string MOB_TBL         = "gamedata/mobs/mobs.tbl";
const std::string MOB_LOOT_DIR    = "gamedata/mobs/loot/";

/* starting player information */
const std::string STARTING_MAP = "floor001";
const std::string STARTING_WPN = "l_sword";
const std::string STARTING_AMR = "starter_scale_mail";

/* quest target information */
const std::string QUEST_TARGET_ID = "mino01";
const std::string QUEST_TARGET_FLOOR = "floor007";
const Coord QUEST_TARGET_COORD = Coord(4,7);

/* gameplay constants */
const int MAX_DAYS = 5;

class Game{
  private:
    std::vector<std::string> messages;      /* container of messages to print per round */
    std::map<std::string, Floor*> floors;   /* game floors */
    std::map<std::string, Item*> items;     /* game item data */
    std::map<std::string, mob_data*> mobs;  /* map of mobID to data */

    Player player;                          /* the player character */
    Mob *quest_target;                      /* */
    bool in_progress;                       /* whether the game is in progress */
    Floor *current_floor;                   /* pointer to the current floor */
    std::ofstream logfile;                  /* logfile */

    int days_passed;
    
  public:
    /* constructors destructors */
    Game(std::string hero_name);
    ~Game();

    /* methods for loading game objects */
    void load_floors();
    void link_spaces();
    void load_items();
    void load_mobs();

    /* player-related methods */
    void move_player(const direction &dir);
    void manage_player_inventory();
    Item* get_player_inventory_selection(const char *prompt);
    void print_player_inventory();
    void player_get_items();
    void player_drop_item();
    void player_examine_item();
    void player_equip_item();
    void player_attack_mob(Character *mob);
    void player_rest();
    void print_player_character_sheet();

    /* monster-related methods */
    void move_mobs();
    bool mob_make_moves(Character *mob, const std::vector<direction> &moves);
    bool mob_attack_player(Character *mob);
   
    /* misc. game methods */
    void read_input(int input);
    void inc_day() { this->days_passed++; }
    std::string render();
    std::string print_status_bar();
    Coord coord_from_direction(const Coord &coord, const direction &dir);
    bool is_in_progress() { return this->in_progress; }
};

#endif
