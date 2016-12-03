#include "Floor.hpp"
#include "Character.hpp"

void Floor::load_floor(std::string path)
{
  std::ifstream in_file(path.c_str());
  std::string line;
  Coord coord(0,0);

  while(std::getline(in_file, line)){
    coord.set_x(0);
    for(int i = 0; i < line.length(); i++){      
      this->spaces.insert(std::pair<Coord,Space*>(coord, interpret_space(line.at(i), coord)));
      coord.set_x(coord.x() + 1);
    }
    coord.set_y(coord.y() + 1); 
  }
  in_file.close();
}

Space *Floor::interpret_space(char space_char, Coord coord)
{
  Space *space = NULL;

  switch(space_char) {
   case EMPTY_SPACE_C:
      space = new OpenSpace(coord);
      break;

   case CLOSED_DOOR_C:
      space = new Door(coord);
      break;

    case WALL_C:
      space = new Wall(coord);
      break;

    case UP_STAIR_C:
      space = new UpStair(coord);
      break;

    case DOWN_STAIR_C:
      space = new DownStair(coord);
      break;
    
    case HIDDEN_DOOR_C:
      space = new SecretDoor(coord);
      break;
  }

  return space;
}

std::string Floor::render_floor()
{
  std::string render_string = "";

  int last_y = 0,
      y = 0;

  for(auto i = this->spaces.begin(); i != this->spaces.end(); i++){
    y = i->second->y();

    if (y != last_y){      
      last_y = y;
      render_string += '\n';
    }
    render_string += i->second->get_render_char();
  }
  
  return render_string;
}

Floor::~Floor()
{
  for(auto i = this->spaces.begin(); i != this->spaces.end(); i++){
    delete (i->second);
  }
  for(auto i = this->mob_list.begin(); i != this->mob_list.end(); i++){
    delete (*i);
  }
}

bool Floor::add_char(Character *character, const Coord &coord)
{
  return this->spaces[coord]->add_character(character);
}

bool Floor::move_char(const Coord &from, const Coord &to)
{
  Character *character = NULL;
  bool moved = false;

  if ((this->spaces[from]->get_character() != NULL) && (this->spaces[to]->get_character() == NULL)){    
    character = this->spaces[from]->get_character();
    if (character != NULL &&
        this->spaces[to]->add_character(character) &&
        this->spaces[from]->delete_character()) {
      character->set_coord(to);
      moved = true;
    }
  }
  
  return moved;
}
