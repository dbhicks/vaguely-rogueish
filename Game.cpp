/*************************************************************************
 * Program File: Game.cpp
 * Author: David Bacher-Hicks
 * Date: 3 December 2016
 * Description: A game object that handles operating a roguelike game from
 *              loading all game data, handling input and managing player
 *              and non-player characters.
 *
 * Input: Reads gamedata from gamedata files
 *        Character input on Game::read_input(char)
 * Output: standard out
 ************************************************************************/

#include <ncurses.h>
#include <typeinfo>
#include <iomanip>
#include <set>
#include "Game.hpp"
#include "utils.hpp"

/*************************************************************************
 * Function: Constructor 
 * Description: Loads gamedata and initializes the gamestate for play.
 * Parameters:  1) accepts a player name
 * Pre-conditions: 
 * Post-conditions:
 * Returns: none
 ************************************************************************/
Game::Game(std::string hero_name)
{  
  /* open logfile for logging */
  this->logfile.open(LOGFILE_PATH.c_str());

  /* Load data */
  this->logfile << "Loading items...\n";
  load_items();
  this->logfile << "Finished loading items.\n\n";
  
  this->logfile << "Loading mobs...\n";
  load_mobs();
  this->logfile << "Finished loading mobs.\n\n";

  this->logfile << "Loading floors...\n";
  load_floors();
  this->logfile << "Finished loading floors.\n\n";  
  
  /* 
   * Set initial game conditions 
   *  Set the player to the global constant starting map and location,
   *  add and equip the global constant starting equipment,
   *  set the game to in progress and the number of days passed to 0
   */
  this->current_floor = this->floors[STARTING_MAP];
  this->player.set_name(hero_name);
  this->player.set_coord(STARTING_COORD.x(), STARTING_COORD.y());
  this->player.add_item(this->items[STARTING_WPN]);
  this->player.add_item(this->items[STARTING_AMR]);
  this->player.equip_item(this->items[STARTING_WPN]);
  this->player.equip_item(this->items[STARTING_AMR]);
  this->current_floor->add_char(&player, STARTING_COORD);

  this->in_progress = true;
  this->days_passed = 0;

  this->logfile << "Game ready!\n";
  this->logfile.close();
}


/*************************************************************************
 * Function: Destructor
 * Description: Cleans up dynamically allocated memory
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the dynamically allocated memory will have been deallocated
 * Returns: none
 ************************************************************************/
Game::~Game()
{
  for( auto i = floors.begin(); i != floors.end(); i++ ) {
    delete i->second;
    i->second = NULL;
  }
  for( auto i = items.begin(); i != items.end(); i++ ) {
    delete i->second;
    i->second = NULL;
  }
  for ( auto i = mobs.begin(); i != mobs.end(); i++ ) {
    delete i->second;
    i->second = NULL;
  }

  this->logfile.close();
}


/*************************************************************************
 * Function: read_input
 * Description: accepts and handles character input as an integer parameter
 * Parameters: int input (characters including ncurses special characters)
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: none
 ************************************************************************/
void Game::read_input(int input)
{
  switch(input){
    case KEY_UP:
    case 'w':
      move_player(UP);
      break;

    case KEY_LEFT:
    case 'a':
      move_player(LEFT);
      break;

    case KEY_RIGHT:
    case 'd':
      move_player(RIGHT);
      break;

    case KEY_DOWN:
    case 's':
      move_player(DOWN);
      break;

    case 'g':
      player_get_items();
      break;

    case 'i':
      manage_player_inventory();
      break;

    case 'r':
      player_rest();
      break;

    case 'c':
      print_player_character_sheet();
      break;

    case 'Q':
      this->in_progress = false;
      break;
  }
}


/*************************************************************************
 * Function: coord_from_direction
 * Description: returns a coordinate to the &dir of the passed coordinate
 * Parameters: 1) const Coord &coord - origin coordinate
 *             2) const direction &dir - the direction of the origin
 *
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: Coordinate, offset from the passed coordinate one cell in the
 *          passed direction.
 ************************************************************************/
Coord Game::coord_from_direction(const Coord &coord, const direction &dir)
{
  int dx = 0,
      dy = 0;

  if(dir == UP){
    --dy;
  } else if (dir == DOWN) {
    ++dy;
  } else if (dir == LEFT) {
    --dx;
  } else if (dir == RIGHT){
    ++dx;
  }
  
  return Coord( (coord.x()+dx), (coord.y()+dy) );
}


/*************************************************************************
 * Function: player_rest
 * Description: rests the player if the current floor is empty. Otherwise,
 *              prints a message that the character cannot rest due to the
 *              nearby monsters.
 *
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the player may have had its hp replenished and the 
 *                  day may have been advanced. 
 * Returns: none
 ************************************************************************/
void Game::player_rest()
{
  std::stringstream ss;
  if (this->current_floor->get_mob_list()->size() == 0) {
    this->inc_day();
    ss << "You rest and recover health.\n"
       << "You've now been in the dungeon for " << this->days_passed 
       << " days.\n";
    this->player.rest();
    if (this->days_passed > MAX_DAYS) {
      ss << "You've taken too long to clear the dungeon.\n";
    }    
  } else {
    ss << "You cannot rest while there are monsters nearby.\n";
  }

  this->messages.push_back(ss.str());
}


/*************************************************************************
 * Function: player_get_items
 * Description: attempt to get items from the space the player currently
 *              occupies. If there are items, print a confirmation message
 *              for each item prior to getting.
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the items may have been removed from the space and added
 *                  to the player's inventory.
 *
 * Returns: none
 ************************************************************************/
void Game::player_get_items()
{
  /*  Get the player's space and the items present at that space. */
  Space *space = this->current_floor->get_space(player.get_coord());
  std::vector<Item*> *itm = space->get_items();
  std::vector<std::string> removed_item_ids;

  /* 
   * If there are items, then for each item, 
   *  ask if the player wants to get the item,
   *    then add the item to the player's inventory and record its id to remove
   *    from the space
   */
  if( itm->size() > 0) {
    for( auto i = itm->begin(); i != itm->end(); i++ ){
      printw("Get the ");
      printw((*i)->name().c_str());
      printw("? y/n\n");
      refresh();
      if ( getch() == 'y' ) {
        printw("You got the %s\n", (*i)->name().c_str());
        refresh();

        player.add_item(*i);
        removed_item_ids.push_back( (*i)->id() );
      }
    }

    printw("Press any key to continue...");
    getch();
  
    /* remove all taken items from the space */
    for( auto i = removed_item_ids.begin(); i != removed_item_ids.end(); i++) {
        space->remove_item( *i );
    }
  } else {
    this->messages.push_back("There are no items to get\n");
  }
}


/*************************************************************************
 * Function: print_player_character_sheet
 * Description: displays a character sheet with information like stats and
 *              experience
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: none
 ************************************************************************/
void Game::print_player_character_sheet()
{
  clear();
  printw("Character Name: %s\n", player.get_name().c_str());
  printw("Level: %i\n", player.get_level());
  printw("Experience: %i\n", player.get_experience());
  printw("Base Attack: %i\n", player.get_b_atk()); 
  printw("Stats:\n");
  printw("  STR: %i (%i)", player.get_ability(STR), player.get_ability_mod(STR));
  printw("  DEX: %i (%i)\n", player.get_ability(DEX), player.get_ability_mod(DEX));
  printw("  CON: %i (%i)", player.get_ability(CON), player.get_ability_mod(CON));
  printw("  INT: %i (%i)\n", player.get_ability(INT), player.get_ability_mod(INT));
  printw("  WIS: %i (%i)", player.get_ability(WIS), player.get_ability_mod(WIS));
  printw("  CHA: %i (%i)\n", player.get_ability(CHA), player.get_ability_mod(CHA));
  printw("\n\nPress any key to continue\n");
  getch();
}


/*************************************************************************
 * Function: manage_player_inventory
 * Description: manages the player's inventory, displaying the inventory
 *              and allowing the user to select various choices, including
 *              equipping items and dropping items.
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the character's equipment and inventory may have been changed
 * Returns: none
 ************************************************************************/
void Game::manage_player_inventory()
{
  std::map<Item *, int> *inventory = player.get_inventory(); /* player inventory */
  char choice = ' ';
  int indx = 0;

  /*
   *  Loop to accept input until return is selected or the inventory has been emptied
   *    display the inventory, then accept an operation and branch appropriately
   */
  while (choice != 'r' && inventory->size() > 0) {
    if (inventory->size() > 0){

      clear();
      this->print_player_inventory();
      printw("\nYou may perform an inventory operation or return to play.\n");
      printw("Possible operations: \n");
      printw("\td - drop an item\n");
      printw("\te - equip an item\n");
      printw("\tx - examine an item\n");
      printw("\tr - return to play\n\n");

      printw("You are carrying %i / %i lbs.\n", 
            static_cast<int>(player.carry_weight()), 
            static_cast<int>(player.max_carry()));

      printw("Upon reviewing your inventory, you decide to...\n");
      curs_set(1);
      refresh();
      choice = getch();

      switch(choice){
        case 'd':
          this->player_drop_item();
          break;

        case 'e':
          this->player_equip_item();
          break;

        case 'x':
          this->player_examine_item();
          break;

        case 'r':
          break;
          
        default:
          printw("That is not an understood inventory operation.\n");
          printw("Press any key to continue...");
          getch();
        break;
      }
    } else {
      this->messages.push_back("You aren't carrying anything\n");
    }
  }

  curs_set(0);
}


/*************************************************************************
 * Function: print_player_inventory
 * Description: prints the player's inventory
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: none
 ************************************************************************/
void Game::print_player_inventory()
{ 
  std::map<Item*, int> *inventory = player.get_inventory();
  char idx = 'a'; /* a character to identify each item */

  printw("You are currently carrying:\n");

  for( auto i = inventory->begin(); i != inventory->end(); i++ ){
    printw("%c) ",  idx++); /* increment the identifying character */
    printw(i->first->name().c_str()); /* print the item name */
    if (i->second > 1) {              /* and quantity, if more than one */
      printw("(%i)", i->second);
    }
    printw("\n");
  }
}


/*************************************************************************
 * Function: get_player_inventory_selection
 * Description: gets a user selection for an item, returns a pointer to it
 * Parameters: const char *prompt - the prompt to display to the user
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: pointer to an item chosen by the user (NULL if cancelled)
 ************************************************************************/
Item* Game::get_player_inventory_selection(const char *prompt)
{
  clear();
  std::map<Item *, int> *inventory = player.get_inventory();
  int selection = ' ';
  Item* inventory_item = NULL;
  this->print_player_inventory();

  /*
   *  display a prompt, then loop to validate input.
   *    Align character input with map "index" by adjusting by 'a'
   */
  printw(prompt);
  while(((selection = getch() - 'a') < 0 || selection >= inventory->size()) && 
                                            (selection + 'a' != ' ')){

    printw("Invalid selection. Please try again, or press space to cancel.\n");
    printw(prompt);
  }
  
  /* 
   *  unless cancelled, advance the map iterator to the selected item,
   *    then return it
   */
  if ( (selection + 'a') != ' ' ) { 
    auto it = inventory->begin();
    std::advance(it, selection);
    inventory_item = it->first;
  }

  return inventory_item;
}


/*************************************************************************
 * Function: player_drop_item
 * Description: gets an item selection and attempts to drop the item.
 *              I'm not dealing with bare-handed combat, so you can't
 *              drop weapons or armor currently.
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: an item may have been removed from the player's inventory
 * Returns: none
 ************************************************************************/
void Game::player_drop_item()
{
  clear();
  /* print the inventory and a prompt. Get an item selection */
  this->print_player_inventory();
  Item *item = this->get_player_inventory_selection("Drop which item?\n");
  refresh();
  
  /* if an item was selected (not cancelled) then attempt to remove it */
  if (item != NULL){

    /* disallow if it's an equipped weapon or armor (w/o duplicates) */
    if ( ( item == player.get_weapon() || item == player.get_armor() ) &&
        ( player.item_count(item) == 1 ) ){
      printw("You can't drop equipped items.\n");

    /* otherwise drop it  */
    } else {
      printw("You drop the %s\n", item->name().c_str());
      player.remove_item(item);
      this->current_floor->get_space(player.get_coord())->add_item(item);
    }
  }

  printw("Press any key to continue...\n");
  refresh();
  getch();
}    


/*************************************************************************
 * Function: player_equip_item
 * Description: equip an item
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the item may have been equipped
 * Returns: none
 ************************************************************************/
void Game::player_equip_item()
{
  clear();
  /* print the inventory and get a selection */
  this->print_player_inventory();
  Item *item = this->get_player_inventory_selection("Equip which item?\n");
  refresh();

  /* equip  if possible or display a failure message */
  if (typeid(*item) == typeid(Armor) || typeid(*item) == typeid(Weapon)){
    player.equip_item(item);
    printw("You equipped the %s\n", item->name().c_str());
  } else {
    printw("You can't equip that!\n");
  }

  printw("Press any key to continue...\n");
  refresh();
  getch();
}


/*************************************************************************
 * Function: player_examine_item
 * Description: display an item's description
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: none
 ************************************************************************/
void Game::player_examine_item()
{
  clear();
  /* print the inventory and get a selection */
  this->print_player_inventory();
  Item *item = this->get_player_inventory_selection("Examine which item?\n");
  refresh();  

  /* if the selection wasn't cancelled, print the description */
  if ( item != NULL ){
    printw("%s\n", item->description().c_str());
  }

  printw("Press any key to continue...\n");
  refresh();
  getch();
}


/*************************************************************************
 * Function: move_player
 * Description: the main function governing movement of the player character
 *              moves the player or contextually handles other actions,
 *              including opening doors, traversing stairs, and attacking
 *              monsters.
 *
 * Parameters: 1) const direction &dir - the direction to move the player
 * Pre-conditions: none
 * Post-conditions: the player may have been moved or other actions
 *                  may have occurred.
 * Returns: none
 ************************************************************************/
void Game::move_player(const direction &dir)
{
  Coord from = this->player.get_coord();
  Coord to = coord_from_direction(from, dir);
  Space *to_space = this->current_floor->get_space(to.x(), to.y());

  /* if the space is not empty, then attack the character present there */
  if ( to_space->get_character() != NULL ) {
      this->player_attack_mob( to_space->get_character() );

  /* otherwise, if there are no characters present */
  } else {

    /* if passable, move the player there */
    if(to_space->passable()){
      this->current_floor->move_char(from, to);
    
    /* if not passable, check for other actions */
    } else {

      /* walls */
      if ( typeid(*to_space) == typeid(Wall)){
        this->messages.push_back("There is a wall blocking the way...");

      /* 
       * doors - check to see if locked. If not, then open. If so, then
       *  check to see if the PC holds the key. If so, open. 
       */
      } else if ( typeid(*to_space) == typeid(Door) ) {
        this->messages.push_back("There is a door blocking the way...");

        if (to_space->is_locked()){
          this->messages.push_back(" and it's locked.\n");
          std::string key_ID = dynamic_cast<Door*>(to_space)->key_str();

          if ( this->player.has(this->items[key_ID])) {
            this->messages.push_back("You have the key, so you unlock and open the door.\n");
            dynamic_cast<Door*>(to_space)->open();

          } else {
            this->messages.push_back("You do not have the key.\n");
          }

        } else {
          this->messages.push_back(" but it's not locked.\nYou open the door.");
          dynamic_cast<Door*>(to_space)->open();
        }

      /* secrets (sshhhh) */
      } else if ( typeid(*to_space) == typeid(SecretDoor) ){
        this->messages.push_back("There is a wall blocking the way...\n"
                                 "on closer inspection, you find a switch embedded in the wall.\n"
                                 "Pressing the switch reveals a secret passage.\n");
        dynamic_cast<SecretDoor*>(to_space)->open();
      }
    }

    /* 
     * perform conditional checks after movement. 
     *  If there are items in the destination space,
     *    notify the user. 
     *  If the space is a stair,
     *    traverse the stair
     */

    /* check for and notify re:items */
    std::vector<Item*>* items = to_space->get_items();
    if (items->size() > 0) {
      this->messages.push_back("There are items here:\n");
      for( auto i = items->begin(); i != items->end(); i++ ){
        this->messages.push_back(std::string("\t") + (*i)->name() + std::string("\n") );
      }
    }

    /* check for and traverse stairs */
    if (typeid(*to_space) == typeid(DownStair)) {
      this->messages.push_back("You descend the stairs to a deeper level...\n");
      to_space->delete_character();
      player.set_coord(dynamic_cast<Stair*>(to_space)->get_linked_coord());
      this->current_floor = this->floors[dynamic_cast<Stair*>(to_space)->get_linked_floor_ID()];
      this->current_floor->get_space(player.get_coord())->add_character(&player);

    } else if (typeid(*to_space) == typeid(UpStair)) {
      this->messages.push_back("You ascend the stairs to a higher level...\n");
      to_space->delete_character();
      player.set_coord(dynamic_cast<Stair*>(to_space)->get_linked_coord());
      this->current_floor = this->floors[dynamic_cast<Stair*>(to_space)->get_linked_floor_ID()];
      this->current_floor->get_space(player.get_coord())->add_character(&player);
    }
  }

  /* a movement related action triggers a mob turn */
  move_mobs();
}


/*************************************************************************
 * Function: player_attack_mob
 * Description: handles attacks from the player on monsters
 * Parameters: mob - the monster being attacked
 * Pre-conditions: none
 * Post-conditions: the monster may have had its hp adjusted and may
 *                  have been removed / deleted if killed. The player
 *                  character may have been awarded experience.
 * Returns: none
 ************************************************************************/
void Game::player_attack_mob(Character *mob)
{
  std::stringstream attack_string; /* stringstream to accumulate a message */

  /* Describe a hit or miss. */
  attack_string << "You attack " << mob->get_name();
  attack_data atk = player.attack();

  if(mob->defend(atk)){
    attack_string << " for "
                  << atk.damage_roll
                  << " damage.\n";
  } else {
    attack_string << " but miss.\n";
  }

  /* push and clear the message */
  this->messages.push_back(attack_string.str());
  attack_string.str("");

  /* 
   * check to see if the monster has been killed,
   *  if so, display another message, 
   *    drop the mob's inventory into its last known location,
   *    award the PC some experience,
   *    remove the mob from the space,
   *    unlist it from the floor's listing,
   *    and free the memory.
   */
  if (dynamic_cast<Mob*>(mob)->is_dead()) {
    attack_string << "You have slain "
                  << mob->get_name()
                  << '\n';

    this->messages.push_back(attack_string.str());
    attack_string.str("");

    Space *space = this->current_floor->get_space(mob->get_coord());
    
    for (auto i = mob->get_inventory()->begin(); i != mob->get_inventory()->end(); i++) {
      space->add_item(i->first);
    }

    int exp = dynamic_cast<Mob*>(mob)->get_experience();
    attack_string << "You have gained " << exp << " experience!\n";
    this->messages.push_back(attack_string.str());

    player.add_experience(dynamic_cast<Mob*>(mob)->get_experience());

    if (space->delete_character()){
      this->current_floor->unlist_mob(mob);
      delete mob;
    }
  }
} 


/*************************************************************************
 * Function: render
 * Description: renders the floor
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: string rendering of the floor
 ************************************************************************/
std::string Game::render()
{
  std::string render_str = this->current_floor->render_floor();
  render_str += '\n';
  render_str += this->print_status_bar();
  render_str += "\n\n";

  while( this->messages.size() > 0 ){
    render_str += this->messages.front();
    this->messages.erase(this->messages.begin());
  }

  return render_str;
}


/*************************************************************************
 * Function: print_status_bar
 * Description: prints a status bar with information about the character
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: string rendering of a status bar
 ************************************************************************/
std::string Game::print_status_bar()
{
  std::stringstream status_bar;

  status_bar << player.get_name()
             << std::setw(28) << "HP:"
             << std::setw(4) << player.get_hp() << '/'
             << std::setw(4) << player.get_max_hp();

  return status_bar.str();
}


/*************************************************************************
 * Function: move_mobs
 * Description: handles pathing and moving of the monsters on the current floor
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: The monsters may have moved or attacked. 
 *                  The player's hp may have been adjusted and might be dead.
 * Returns: none
 ************************************************************************/
void Game::move_mobs()
{
  /* 
   * get a listing of the monsters on the floor and 
   *  construct two vectors of moves:
   *    1) priority moves (taking the monster closer to the player, or
   *    2) secondary moves, made only if the monster cannot advance directly to
   *       the player.
   *  It's no dijkstra, but whoever said goblins were smart?
   */
  std::set<Character *> *mob_list = this->current_floor->get_mob_list();
  std::vector<direction> primary_moves;
  std::vector<direction> secondary_moves;
  
  Coord player_coord = player.get_coord();
  Coord mob_coord;

  for (auto i = mob_list->begin(); i != mob_list->end(); i++){
    mob_coord = (*i)->get_coord();
    
    if ( player_coord.x() < mob_coord.x() ) {
      primary_moves.push_back(LEFT);
      secondary_moves.push_back(RIGHT);
    } else if ( player_coord.x() > mob_coord.x() ) {
      primary_moves.push_back(RIGHT);
      secondary_moves.push_back(LEFT);
    } else {
      secondary_moves.push_back(LEFT);
      secondary_moves.push_back(RIGHT);
    }

    if ( player_coord.y() < mob_coord.y() ) {
      primary_moves.push_back(UP);
      secondary_moves.push_back(DOWN);
    } else if ( player_coord.y() > mob_coord.y() ) {
      primary_moves.push_back(DOWN);
      secondary_moves.push_back(UP);
    } else {
      secondary_moves.push_back(UP);
      secondary_moves.push_back(DOWN);
    }

    if ( !mob_make_moves(*i, primary_moves) ) {
      mob_make_moves(*i, secondary_moves);
    }

    while( primary_moves.size() > 0 ) {
      primary_moves.pop_back();
    }
    while( secondary_moves.size() > 0) {
      secondary_moves.pop_back();
    }
  }
}


/*************************************************************************
 * Function: mob_make_moves
 * Description: attempts to move the passed monster in the passed directions
 *              in random order until a move was made or all moves were tried.
 * Parameters:  1) Character *mob - the monster to move
 *              2) vector<direction> &moves - the moves to try
 *            
 * Pre-conditions: none
 * Post-conditions: the monster may have been moved or may have attacked.
 * Returns: true on success (a move was made) or false on failure (no moves)
 ************************************************************************/
bool Game::mob_make_moves(Character *mob, const std::vector<direction> &moves)
{
  bool moved = false;
  Coord space;

  if (moves.size() != 0){
    int i = rand() % moves.size(),
        j = 0;
    
    while ( j < moves.size() && !moved ) {
      space = coord_from_direction(mob->get_coord(), moves.at((i+j) % moves.size()));
      if (player.get_coord() == space){
        mob_attack_player(mob);
        moved = true;
      } else if ( this->current_floor->move_char(mob->get_coord(), space)) { 
        moved = true;
      }
      j++;
    }
  }

  return moved;
}


/*************************************************************************
 * Function: mob_attack_player
 * Description: handles attacks of monsters on the player character
 * Parameters: mob - the attacking monster
 * Pre-conditions: none
 * Post-conditions: the player's hp may have been adjusted and may be dead.
 * Returns: true, if hit. False, if missed.
 ************************************************************************/
bool Game::mob_attack_player(Character *mob)
{
  bool hit = false;

  /* 
   * conditional check prevents yet to move monsters 
   * from beating a dead player character while waiting for gameover at next
   * main loop iteration.    
   *
   * If the player is indeed alive, build an attack and deliver it to the 
   * PC's defense function. Describe the effect and check for a dead player.
   */
  if (!player.is_dead()){     
    std::stringstream ss;
    ss << mob->get_name() << " attacks you ";

    attack_data atk = mob->attack();
    if ( player.defend(atk) )  {
      ss << "and hits for " << atk.damage_roll << " damage.\n";
      hit = true;
    } else {
      ss << "but it misses.\n";
    }

    this->messages.push_back(ss.str());
    
    if ( player.is_dead() ) {
      player.set_hp(0);
      this->messages.push_back("You have died!\n");
      this->in_progress = false;
    }
  }

  return hit;
}


/*************************************************************************
 * Function: load_floors
 * Description: loads the floor data
 * Parameters: none
 * Pre-conditions: item data and monster data must have been loaded first
 * Post-conditions: the floor data will have been loaded
 * Returns: none
 ************************************************************************/
void Game::load_floors()
{
  std::stringstream map_path_ss;  /* map path */
  std::stringstream data_path_ss; /* map data path */
  std::ifstream map_data_file;    /* map data file stream */
  std::string map_id;             /* map_id value */
  Space *space;                   /* space variable for interacting with spaces during load */
  std::string data_object;        /* string to hold object type */
  std::string tgt_id;             /* string to hold target (monster, item, weapon, etc) ID */
  int obj_x, obj_y,               /* Coordinate variables... */
      coord_x, coord_y; 
  std::string line;               /* string to hold input lines streamed */
  std::stringstream line_ss;      /* stringstream to stream from individual lines */
  Floor *new_floor;               /* temp new floor variable, holds prior to insertion */

  /*
   *  Initialize the map number to 1 and increment while looping through all existing files,
   *  in the map path root following the naming convention "floor_XX.mp/.dat"
   *
   *  Parse each pair into a Floor object, made up of spaces, containing monsters and items.
   */
  int map_num = 1;
  map_path_ss   << MAP_PATH_ROOT << "floor_" << map_num << ".mp";
  data_path_ss  << MAP_PATH_ROOT << "floor_" << map_num << ".dat";

  while(file_exists(map_path_ss.str().c_str()) && file_exists(data_path_ss.str().c_str())){
 
    /* make a new Floor, load the floor layout into it, then open the datafile and begin parsing */
    new_floor = new Floor;
    new_floor->load_floor(map_path_ss.str().c_str()); 
    map_data_file.open(data_path_ss.str().c_str());
    map_data_file >> map_id;    
    std::getline(map_data_file, line);
    
    this->logfile << "\tLoading \"" << map_id << "\"\n";

    /* 
     * while there are entries in the data file, 
     *  read the associated object identifier and coordinate location
     *  then branch based on type
     */
    while(std::getline(map_data_file, line)){
      line_ss.clear();
      line_ss.str(line);
      line_ss >> data_object >> obj_x >> obj_y;
      space = new_floor->get_space(obj_x, obj_y);
      
      /* doors. If they have an entry, they're locked */
      if (data_object == "door") {
        if (line_ss >> tgt_id) {
          dynamic_cast<Door*>( space )->set_key( tgt_id );
        }
      
      /* stairs, linked to another stair on another floor  */
      } else if (data_object == "stair") {
        line_ss >> tgt_id >> coord_x >> coord_y;
        dynamic_cast<Stair*>( space )->set_linked_floor_ID( tgt_id );
        dynamic_cast<Stair*>( space )->set_linked_coord(Coord(coord_x, coord_y));

      /* items from the loaded item map */
      } else if (data_object == "item") {
        line_ss >> tgt_id;
        space->add_item( this->items[tgt_id] );        

      /* monsters from  the loaded monster data map */
      } else if (data_object == "mob") {

        line_ss >> tgt_id;
        this->logfile << "\tread " << tgt_id << " -> " << mobs[tgt_id]->name << '\n';

        Character *mob = new Mob(this->mobs[tgt_id], Coord(obj_x, obj_y));
        std::vector<std::pair<std::string, int>> *loot_table;
        loot_table = &(this->mobs[tgt_id]->loot);

        /* determine monster inventory based on the loaded loot table entry for the mob (by ID) */
        for (auto i = loot_table->begin(); i != loot_table->end(); i++){
          if ((rand() % 100 + 1) <= i->second){
            this->logfile << "\t\tgiving " << tgt_id << " " << i->first << '\n';            
            mob->add_item(this->items[i->first]);
          }
        }

        /* list the monster on its floor and add it to the correct space */
        new_floor->list_mob(mob);
        space->add_character(mob);
      }
    }

    /* inser the floor into the map for lookups based on ID and safekeeping until freeing */
    this->floors.insert(std::pair<std::string, Floor*>(map_id, new_floor));
    
    /*
     * Close the open data file prior to opening the next,
     * increment the map number, clear both path stringstreams, and load the next paths
     */
    map_data_file.close();
    map_num++;
    map_path_ss.str("");
    data_path_ss.str("");
    map_path_ss   << MAP_PATH_ROOT << "floor_" << map_num << ".mp";
    data_path_ss  << MAP_PATH_ROOT << "floor_" << map_num << ".dat";
  }    
}


/*************************************************************************
 * Function: load items
 * Description: loads item data from a table into a map, using item ID as
 *              a key. 
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: item data is loaded into Game::items
 * Returns: none
 ************************************************************************/
void Game::load_items()
{
  std::ifstream item_table(ITEM_TBL_PATH.c_str());  /* input file stream for item data */
  std::string item_ID,          /* a string to hold the item ID  */
              item_name,        /* a string to hold the item name */
              item_desc,        /* a string to hold the item description */
              line;             /* a string to hold line by line input */
  std::stringstream line_ss;    /* stringstream to stream line by line   */ 
  double item_weight,           /* doubles to hold item weight and value */
         item_value;

  int i;  /* an integer to use as an index value for custom parser location */


  /*
   *  Loop through all lines in the item table,
   *    load each item's information, 
   *      log the item, 
   *        and insert the item into the Game::items map
   *    Repeat for weapon class and armor class items
   */

  /* generic items */
  while( std::getline(item_table, line) ){
    i = 0;

    item_ID = str_parse_string(line, i);
    item_name = str_parse_string(line, i);
    item_desc = str_parse_string(line, i);
    item_weight = str_parse_double(line, i);
    item_value = str_parse_double(line, i);     

    this->logfile << "\tLoading generic item, " << item_ID << '\n';
    this->items.insert(std::pair<std::string, Item*>
      (item_ID, new Item(item_ID, item_name, item_desc, item_weight, item_value)));  
  }

  item_table.close();
  
  int damage_die_n,
      damage_die_sides,
      damage_die_mod;

  item_table.open(WPN_TBL_PATH.c_str());

  /* weapon class items */
  while( std::getline(item_table, line) ) {
    i = 0;
    item_ID = str_parse_string(line, i);
    item_name = str_parse_string(line, i);
    item_desc = str_parse_string(line, i);
    item_weight = str_parse_double(line, i);
    item_value = str_parse_double(line, i);

    damage_die_n = int(str_parse_double(line, i));
    damage_die_sides = int(str_parse_double(line, i));
    damage_die_mod = int(str_parse_double(line, i));
    this->logfile << "\tLoading weapon, " << item_ID << '\n';
    this->items.insert(std::pair<std::string, Item*>(item_ID, 
    new Weapon( item_ID, 
                item_name, 
                item_desc, 
                item_weight, 
                item_value, 
                damage_die_n, 
                damage_die_sides, 
                damage_die_mod)));  
  }

  item_table.close();
  
  item_table.open(AMR_TBL_PATH);

  int ac;
  
  /* armor class items */
  while( std::getline(item_table, line) ) {
    i = 0;
    item_ID = str_parse_string(line, i);
    item_name = str_parse_string(line, i);
    item_desc = str_parse_string(line, i);
    item_weight = str_parse_double(line, i);
    item_value = str_parse_double(line, i);  
    ac = int(str_parse_double(line, i));
    this->logfile << "\tLoading armor, " << item_ID << '\n';
    this->items.insert(std::pair<std::string, Item*>
    (item_ID, new Armor(item_ID, item_name, item_desc, item_weight, item_value, ac)));  
  }
  item_table.close();  
}


/*************************************************************************
 * Function: load mobs
 * Description: load the monster information into a map of structs holding
 *              the initialization information for monsters of a particular
 *              type, keyed by a unique string ID.
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the monster data is loaded into a map
 * Returns: none
 ************************************************************************/
void Game::load_mobs()
{
  std::ifstream mob_table(MOB_TBL.c_str());
  std::ifstream mob_loot_table;
  std::string line;
  int i;
  std::string loot_id;
  int loot_chance;
  mob_data *data;

  while(std::getline(mob_table, line)) {
    i = 0;

    data = new mob_data;

    data->id   = str_parse_string(line, i);
    data->name = str_parse_string(line, i);
    data->render_char = str_parse_string(line, i)[0];
    data->ac = static_cast<int>(str_parse_double(line, i));
    data->hp = static_cast<int>(str_parse_double(line, i));
    data->die_n = static_cast<int>(str_parse_double(line, i));
    data->die_s = static_cast<int>(str_parse_double(line, i));
    data->die_m = static_cast<int>(str_parse_double(line, i));
    data->b_atk = static_cast<int>(str_parse_double(line, i));
    data->cr = str_parse_double(line, i);

    this->logfile << "\tCreating loot table for " << data->id << '\n';
    mob_loot_table.open((MOB_LOOT_DIR + data->id + std::string(".tbl")).c_str());
    while(std::getline(mob_loot_table, line)){
      i = 0;
      loot_id = str_parse_string(line, i);
      loot_chance = static_cast<int>(str_parse_double(line, i));
      data->loot.push_back(std::pair<std::string, int>(loot_id, loot_chance));
      this->logfile << "\t\tAdded " << loot_id << " with chance " << loot_chance << '\n';
    }

    this->logfile << "\tLoaded mob, " << data->id << '\n';
    this->mobs.insert(std::pair<std::string, mob_data*>(data->id, data));
    mob_loot_table.close();
  }

  mob_table.close();
}
