#include <ncurses.h>
#include <typeinfo>
#include <iomanip>
#include <set>
#include "Game.hpp"
#include "utils.hpp"

Game::Game(std::string hero_name)
{  
  this->logfile.open(LOGFILE_PATH.c_str());

  /* Load data->*/
  this->logfile << "Loading items...\n";
  load_items();
  this->logfile << "Finished loading items.\n\n";
  
  this->logfile << "Loading mobs...\n";
  load_mobs();
  this->logfile << "Finished loading mobs.\n\n";

  this->logfile << "Loading floors...\n";
  load_floors();
  this->logfile << "Finished loading floors.\n\n";  
  
  /* Set initial game conditions */
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

void Game::player_get_items()
{
  Space *space = this->current_floor->get_space(player.get_coord());
  std::vector<Item*> *itm = space->get_items();
  std::vector<std::string> removed_item_ids;

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

    for( auto i = removed_item_ids.begin(); i != removed_item_ids.end(); i++) {
        space->remove_item( *i );
    }

  } else {
    this->messages.push_back("There are no items to get\n");
  }
}

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

void Game::manage_player_inventory()
{
  std::map<Item *, int> *inventory = player.get_inventory();
  char choice = ' ';
  int indx = 0;

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

void Game::print_player_inventory()
{ 
  std::map<Item*, int> *inventory = player.get_inventory();
  char idx = 'a';

  printw("You are currently carrying:\n");

  for( auto i = inventory->begin(); i != inventory->end(); i++ ){
    printw("%c) ",  idx++);
    printw(i->first->name().c_str());
    if (i->second > 1) {
      printw("(%i)", i->second);
    }
    printw("\n");
  }
}

Item* Game::get_player_inventory_selection(const char *prompt)
{
  std::map<Item *, int> *inventory = player.get_inventory();
  clear();
  int selection = ' ';

  Item* inventory_item = NULL;

  this->print_player_inventory();

  printw(prompt);
  while(((selection = getch() - 'a') < 0 || selection >= inventory->size()) && (selection + 'a' != KEY_BACKSPACE)){
    printw("Invalid selection. Please try again, or press backspace to cancel.\n");
    printw(prompt);
  }

  if ( (selection + 'a') != KEY_BACKSPACE ) { 
    auto it = inventory->begin();
    std::advance(it, selection);
    inventory_item = it->first;
  }

  return inventory_item;
}

void Game::player_drop_item()
{
  clear();
  this->print_player_inventory();
  Item *item = this->get_player_inventory_selection("Drop which item?\n");
  refresh();
  
  if (item != NULL){

    if ( ( item == player.get_weapon() || item == player.get_armor() ) &&
        ( player.item_count(item) == 1 ) ){
      printw("You can't drop equipped items.\n");
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

void Game::player_equip_item()
{
  clear();
  this->print_player_inventory();
  Item *item = this->get_player_inventory_selection("Equip which item?\n");
  refresh();

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

void Game::player_examine_item()
{
  clear();
  this->print_player_inventory();
  Item *item = this->get_player_inventory_selection("Examine which item?\n");
  refresh();  

  if ( item != NULL ){
    printw("%s\n", item->description().c_str());
  }

  printw("Press any key to continue...\n");
  refresh();
  getch();
}

void Game::move_player(const direction &dir)
{
  Coord from = this->player.get_coord();
  Coord to = coord_from_direction(from, dir);
  Space *to_space = this->current_floor->get_space(to.x(), to.y());

  if ( to_space->get_character() != NULL ) {
      this->player_attack_mob( to_space->get_character() );

  } else {

    if(to_space->passable()){
      if( this->current_floor->move_char(from, to) ){
        //player.set_coord(to.x(), to.y());
      } 
    } else {
      if ( typeid(*to_space) == typeid(Wall)){
        this->messages.push_back("There is a wall blocking the way...");

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
      } else if ( typeid(*to_space) == typeid(SecretDoor) ){
        this->messages.push_back("There is a wall blocking the way...\n"
                                 "on closer inspection, you find a switch embedded in the wall.\n"
                                 "Pressing the switch reveals a secret passage.\n");
        dynamic_cast<SecretDoor*>(to_space)->open();
      } 
    }

    /* To-space conditional checks after movement */

    std::vector<Item*>* items = to_space->get_items();
    if (items->size() > 0) {
      this->messages.push_back("There are items here:\n");
      for( auto i = items->begin(); i != items->end(); i++ ){
        this->messages.push_back(std::string("\t") + (*i)->name() + std::string("\n") );
      }
    }

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
  move_mobs();
}

void Game::player_attack_mob(Character *mob)
{
  std::stringstream attack_string;
  attack_string << "You attack " << mob->get_name();

  attack_data atk = player.attack();
  if(mob->defend(atk)){
    attack_string << " for "
                  << atk.damage_roll
                  << " damage.\n";
  } else {
    attack_string << " but miss.\n";
  }

  this->messages.push_back(attack_string.str());
  attack_string.str("");

  if (dynamic_cast<Mob*>(mob)->is_dead()) {
    attack_string << "You have slain "
                  << mob->get_name()
                  << '\n';

    this->messages.push_back(attack_string.str());
    Space *space = this->current_floor->get_space(mob->get_coord());
    
    for (auto i = mob->get_inventory()->begin(); i != mob->get_inventory()->end(); i++) {
      space->add_item(i->first);
    }

    int exp = dynamic_cast<Mob*>(mob)->get_experience();
    attack_string.str("");
    attack_string << "You have gained " << exp << " experience!\n";
    this->messages.push_back(attack_string.str());

    player.add_experience(dynamic_cast<Mob*>(mob)->get_experience());

    if (space->delete_character()){
      this->current_floor->unlist_mob(mob);
      delete mob;
    }
  }
} 

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

std::string Game::print_status_bar()
{
  std::stringstream status_bar;

  status_bar << player.get_name()
             << std::setw(28) << "HP:"
             << std::setw(4) << player.get_hp() << '/'
             << std::setw(4) << player.get_max_hp();

  return status_bar.str();
}

void Game::move_mobs()
{
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

bool Game::mob_attack_player(Character *mob)
{
  bool hit = false;
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

void Game::load_floors()
{
  std::stringstream map_path_ss;
  std::stringstream data_path_ss;

  std::ifstream map_data_file;

  std::string map_id;

  Space *space;
  std::string data_object;
  std::string tgt_id;
  int obj_x, obj_y,
      coord_x, coord_y;

  std::string line;
  std::stringstream line_ss;

  Floor *new_floor;

  int map_num = 1;
  map_path_ss   << MAP_PATH_ROOT << "floor_" << map_num << ".mp";
  data_path_ss  << MAP_PATH_ROOT << "floor_" << map_num << ".dat";

  while(file_exists(map_path_ss.str().c_str()) && file_exists(data_path_ss.str().c_str())){
  
    map_data_file.open(data_path_ss.str().c_str());

    map_data_file >> map_id;
    new_floor = new Floor;
    new_floor->load_floor(map_path_ss.str().c_str());
    
    std::getline(map_data_file, line);

    this->logfile << "\tLoading \"" << map_id << "\"\n";
    while(std::getline(map_data_file, line)){
      line_ss.clear();
      line_ss.str(line);

      line_ss >> data_object >> obj_x >> obj_y;
      space = new_floor->get_space(obj_x, obj_y);
      
      if (data_object == "door") {
        if (line_ss >> tgt_id) {
          dynamic_cast<Door*>( space )->set_key( tgt_id );
        }

      } else if (data_object == "stair") {

        line_ss >> tgt_id >> coord_x >> coord_y;

        dynamic_cast<Stair*>( space )->set_linked_floor_ID( tgt_id );
        dynamic_cast<Stair*>( space )->set_linked_coord(Coord(coord_x, coord_y));

      } else if (data_object == "item") {

        line_ss >> tgt_id;
        space->add_item( this->items[tgt_id] );        
      } else if (data_object == "mob") {
        
        line_ss >> tgt_id;
        this->logfile << "\tread " << tgt_id << " -> " << mobs[tgt_id]->name << '\n';
        Character *mob = new Mob(this->mobs[tgt_id], Coord(obj_x, obj_y));

        std::vector<std::pair<std::string, int>> *loot_table;
        loot_table = &(this->mobs[tgt_id]->loot);

        for (auto i = loot_table->begin(); i != loot_table->end(); i++){
          if ((rand() % 100 + 1) <= i->second){
            this->logfile << "\t\tgiving " << tgt_id << " " << i->first << '\n';            
            mob->add_item(this->items[i->first]);
          }
        }

        new_floor->list_mob(mob);
        space->add_character(mob);
      }
    }

    this->floors.insert(std::pair<std::string, Floor*>(map_id, new_floor));

    map_data_file.close();

    map_num++;
    map_path_ss.str("");
    data_path_ss.str("");
    map_path_ss   << MAP_PATH_ROOT << "floor_" << map_num << ".mp";
    data_path_ss  << MAP_PATH_ROOT << "floor_" << map_num << ".dat";
  }    
}

void Game::load_items()
{
  std::ifstream item_table(ITEM_TBL_PATH.c_str());

  std::string item_ID,
              item_name,
              item_desc,
              line;

  std::stringstream line_ss;

  double item_weight,
         item_value;

  int i;

  while( std::getline(item_table, line) ){
    i = 0;

    item_ID = str_parse_string(line, i);
    item_name = str_parse_string(line, i);
    item_desc = str_parse_string(line, i);
    item_weight = str_parse_double(line, i);
    item_value = str_parse_double(line, i);     

    this->logfile << "\tLoading generic item, " << item_ID << '\n';
    this->items.insert(std::pair<std::string, Item*>(item_ID, new Item(item_ID, item_name, item_desc, item_weight, item_value)));  
  }

  item_table.close();
  
  int damage_die_n,
      damage_die_sides,
      damage_die_mod;

  item_table.open(WPN_TBL_PATH.c_str());
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
    this->items.insert(std::pair<std::string, Item*>(item_ID, new Weapon(item_ID, item_name, item_desc, item_weight, item_value, damage_die_n, damage_die_sides, damage_die_mod)));  
  }
  item_table.close();
  
  item_table.open(AMR_TBL_PATH);

  int ac;

  while( std::getline(item_table, line) ) {
    i = 0;
    item_ID = str_parse_string(line, i);
    item_name = str_parse_string(line, i);
    item_desc = str_parse_string(line, i);
    item_weight = str_parse_double(line, i);
    item_value = str_parse_double(line, i);  
    
    ac = int(str_parse_double(line, i));
    this->logfile << "\tLoading armor, " << item_ID << '\n';
    this->items.insert(std::pair<std::string, Item*>(item_ID, new Armor(item_ID, item_name, item_desc, item_weight, item_value, ac)));  
  }
  item_table.close();  
}

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
