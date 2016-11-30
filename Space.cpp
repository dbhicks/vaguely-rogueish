#include "Space.hpp"
#include "Coord.hpp"
#include "Character.hpp"

        ////////////////////////////////////////////////////////
       //                    Space                           //
      ////////////////////////////////////////////////////////
        
Space::Space(const Coord &coord)
{
  this->coord = coord;
  this->present_character = NULL;
  this->linked_spaces.insert(std::pair<direction, Space *>(UP, NULL));
  this->linked_spaces.insert(std::pair<direction, Space *>(RIGHT, NULL));
  this->linked_spaces.insert(std::pair<direction, Space *>(DOWN, NULL));
  this->linked_spaces.insert(std::pair<direction, Space *>(LEFT, NULL));
}

Space::~Space()
{
  for (auto i = items.begin(); i != items.end(); i++){
    delete *i;
    *i = NULL;
  }
}

char Space::get_render_char() const
{
  char c = this->render_char;

  if (this->present_character != NULL) {
    c = this->present_character->get_render_char();
  }  

  return c;
}

bool Space::add_character(Character *character)
{
  bool added_character = false;

  if (this->present_character == NULL && this->passable()){
    this->present_character = character;
    added_character = true;
  }

  return added_character;
}

bool Space::delete_character()
{
  bool deleted = false;

  if ( this->present_character != NULL ) {
    this->present_character = NULL;
    deleted = true;
  }

  return deleted;
}

Character * Space::remove_character()
{
  Character *character = this->present_character;
  this->present_character = NULL;
  return character;
}


        ////////////////////////////////////////////////////////
       //                   OpenSpace                        //
      ////////////////////////////////////////////////////////

bool OpenSpace::passable()
{
  bool pass = true;

  for(auto i = this->items.begin(); (i != this->items.end()) && (pass == true); i++){
    if (!(*i)->passable()) {
      pass = false;
    }
  }

  return pass;
}

Item * OpenSpace::remove_item(std::string item_ID)
{
  Item *item = NULL;
  std::vector<Item*>::iterator it;

  for ( auto i = this->items.begin(); (i != items.end()) && (item == NULL); i++ ){
    if ((*i)->id() == item_ID){
      item = *i;
      it = i;
    }
  }

  if ( item != NULL ){
    this->items.erase(it);
  }

  return item;
}

char OpenSpace::get_render_char() const
{
  char c = this->render_char;

  if ( this->present_character != NULL ) {
    c = this->present_character->get_render_char();
  } else if ( this->items.size() != 0 ) {
    c = ITEM_SPACE_C;
  }

  return c;
}


        ////////////////////////////////////////////////////////
       //                    Door                            //
      ////////////////////////////////////////////////////////

bool Door::open()
{
  bool was_open = this->is_open;
  this->is_open = true;
  return (was_open != this->is_open);
}

bool Door::close()
{
  bool was_open = this->is_open;
  
  if (this->is_open) {
    this->is_open = false;
  }

  return (was_open != this->is_open);
}

char Door::get_render_char() const
{
  char c;

  if ( this->present_character != NULL ) {
    c = this->present_character->get_render_char();
  } else if ( this->items.size() != 0 ) {
    c = ITEM_SPACE_C;
  } else if ( this->is_open ) {
    c = OPEN_DOOR_C;
  } else {
    c = CLOSED_DOOR_C;
  }

  return c;
}


        ////////////////////////////////////////////////////////
       //                    Stair                           //
      ////////////////////////////////////////////////////////


