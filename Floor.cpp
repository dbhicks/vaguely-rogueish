/*************************************************************************
 * Program Filename: Floor.cpp
 * Author: David Bacher-Hicks
 * Date: 3 December 2016
 * Description: A class definition file for a Floor class. Encapsulates
 *              Space objects.
 * Input:  none
 * Output: none
 ************************************************************************/

#include "Floor.hpp"
#include "Character.hpp"

/*************************************************************************
 * Function: load_floor
 * Description: loads a floor layout from a floor layout file
 * Parameters: path - the path of the layout file
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: none
 ************************************************************************/
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


/*************************************************************************
 * Function: interpret_space 
 * Description: interprets a character and creates a new space with that
 *              coordinate information and type.
 *
 * Parameters: space_char - the character to interpret
 *              coord - the coordinate of the space
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: Space *space - a pointer to the created space
 ************************************************************************/
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


/*************************************************************************
 * Function: render_floor
 * Description: renders the floor to a string
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: std::string - the rendered floor
 ************************************************************************/
std::string Floor::render_floor()
{
  std::string render_string = "";
  int last_y = 0,
      y = 0;

  /*
   *  Iterate through the map of spaces and render each character,
   *    render a newline whenever y has changed
   */
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


/*************************************************************************
 * Function:  ~Floor
 * Description: destructor
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the dynamic memory encapsulated within the floor,
 *                  spaces and mob data, will be freed
 * Returns: none
 ************************************************************************/
Floor::~Floor()
{
  for(auto i = this->spaces.begin(); i != this->spaces.end(); i++){
    delete (i->second);
  }
  for(auto i = this->mob_list.begin(); i != this->mob_list.end(); i++){
    delete (*i);
  }
}


/*************************************************************************
 * Function: add_char
 * Description: adds a character to the space at coord
 * Parameters: character - the character to add
 *             coord - the location of the coord to add the character to
 *
 * Pre-conditions: 
 * Post-conditions: the character may have been added to the space
 * Returns: bool - true if the character was successfully added
 ************************************************************************/
bool Floor::add_char(Character *character, const Coord &coord)
{
  return this->spaces[coord]->add_character(character);
}


/*************************************************************************
 * Function: move_char
 * Description: moves a character from one space to another
 * Parameters: from - the coordinate to move from
 *             to - the coordinate to move to
 * Pre-conditions: none
 * Post-conditions: the character may have been moved
 * Returns: bool - true if the character was succesfully moved
 ************************************************************************/
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
