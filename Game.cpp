#include <ncurses.h>
#include <typeinfo>

#include "Game.hpp"
#include "utils.hpp"

Game::Game()
{
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
        this->messages.push_back("and it's locked.");
        if ( this->player.has( dynamic_cast<Door*>(to_space)->key_str() ) ) {
          this->messages.push_back("You do not have the key.");
        } else {
          this->messages.push_back("You have the key, so you unlock and open the door.");
        }
      } else {
        this->messages.push_back("but it's not locked. You open the door.");
        dynamic_cast<Door*>(to_space)->open();
      }
    } 
  }
  
  if (typeid(*to_space) == typeid(DownStair)) {
    this->messages.push_back("You descend the stairs to a deeper level...");    
  } else if (typeid(*to_space) == typeid(UpStair)) {
    this->messages.push_back("You ascend the stairs to a higher level...");
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

  Floor *new_floor;

  int map_num = 1;
  map_path_ss   << MAP_PATH_ROOT << "floor_" << map_num << ".mp";
  data_path_ss  << MAP_PATH_ROOT << "floor_" << map_num << ".dat";

  while(file_exists(map_path_ss.str().c_str()) && file_exists(data_path_ss.str().c_str())){
  
    map_data_file.open(data_path_ss.str().c_str());

    map_data_file >> map_id;
    new_floor = new Floor;
    new_floor->load_floor(map_path_ss.str().c_str());
    this->floors.insert(std::pair<std::string, Floor*>(map_id, new_floor));

    map_data_file.close();

    map_num++;
    map_path_ss.str("");
    data_path_ss.str("");
    map_path_ss   << MAP_PATH_ROOT << "floor_" << map_num << ".mp";
    data_path_ss  << MAP_PATH_ROOT << "floor_" << map_num << ".dat";
  }    
}
