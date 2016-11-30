#include <ncurses.h>
#include <typeinfo>
#include "Game.hpp"
#include "utils.hpp"

Game::Game()
{
  load_items();
  load_floors();

  this->current_floor = this->floors[STARTING_MAP];
  this->player.set_coord(STARTING_COORD.x(), STARTING_COORD.y());
  this->current_floor->add_char(&player, STARTING_COORD);
  this->in_progress = true;
}

Game::~Game()
{
  for( auto i = floors.begin(); i != floors.end(); i++ ) {
    delete i->second;
    i->second = NULL;
  }
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
        printw("You got the ");
        printw((*i)->name().c_str());
        printw(". Press a key to continue...");
        refresh();

        player.add_item(*i);
        removed_item_ids.push_back( (*i)->id() );

        getch();
      }
      printw("You got all the items.\n");
      refresh();
    }

    for( auto i = removed_item_ids.begin(); i != removed_item_ids.end(); i++) {
        space->remove_item( *i );
    }

  } else {
    this->messages.push_back("There are no items to get\n");
  }
}

void Game::move_player(const direction &dir)
{
  Coord from = this->player.get_coord();
  Coord to = coord_from_direction(from, dir);
  Space *to_space = this->current_floor->get_space(to.x(), to.y());

  if(to_space->passable()){
    if( this->current_floor->move_char(from, to) ){
      player.set_coord(to.x(), to.y());
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
    } 
  }

  std::vector<Item*>* items = to_space->get_items();
  if (items->size() > 0) {
    this->messages.push_back("Here thar be items:\n");
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

std::string Game::render()
{
  std::string render_str = this->current_floor->render_floor();
  render_str += "\n\n";

  while( this->messages.size() > 0 ){
    render_str += this->messages.front();
    this->messages.erase(this->messages.begin());
  }

  return render_str;
}

void Game::move_mobs()
{

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
        this->messages.push_back(tgt_id);
        space->add_item( this->items[tgt_id] );        
      }
    }

    this->floors.insert(std::pair<std::string, Floor*>(map_id, new_floor));

    this->messages.push_back("Loaded ");
    this->messages.push_back(map_id);


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
  std::ifstream item_table(ITEM_TBL_PATH);

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

    this->items.insert(std::pair<std::string, Item*>(item_ID, new Item(item_ID, item_name, item_desc, item_weight, item_value)));
  
    this->messages.push_back(item_ID + std::string(" ") + item_name + std::string(" ") + item_desc
                              + std::string(" ") + static_cast<char>(item_weight+'0'));
  }
}


