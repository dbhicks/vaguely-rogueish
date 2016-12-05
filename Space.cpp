/*************************************************************************
 * Program Filenamee: Space.cpp
 * Author: David Bacher-Hicks
 * Date: 3 December 2016
 * Description: A class definition file for a Space class and its subclasses
 *              EmptySpace, Door, Wall, SecretDoor
 * Input:
 * Output:
 ************************************************************************/

#include "Space.hpp"
#include "Coord.hpp"
#include "Character.hpp"

        ////////////////////////////////////////////////////////
       //                    Space                           //
      ////////////////////////////////////////////////////////

/*************************************************************************
 * Function: Space 
 * Description: constructor; initializes the linked spaces and present
 *              character to null
 *
 * Parameters: coord
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: none
 ************************************************************************/
Space::Space(const Coord &coord)
{
  this->coord = coord;
  this->present_character = NULL;
  this->linked_spaces.insert(std::pair<direction, Space *>(UP, NULL));
  this->linked_spaces.insert(std::pair<direction, Space *>(RIGHT, NULL));
  this->linked_spaces.insert(std::pair<direction, Space *>(DOWN, NULL));
  this->linked_spaces.insert(std::pair<direction, Space *>(LEFT, NULL));
}


/*************************************************************************
 * Function: get_render_char
 * Description: returns the character to be rendered for this space
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions:  none
 * Returns: char - character to be rendered.
 ************************************************************************/
char Space::get_render_char() const
{
  char c = this->render_char;

  if (this->present_character != NULL) {
    c = this->present_character->get_render_char();
  }  

  return c;
}


/*************************************************************************
 * Function: add_character
 * Description: adds a character to the space
 * Parameters: character - the character to add
 * Pre-conditions: none
 * Post-conditions: The character will have been added to the space
 * Returns: bool - true if the character was added
 ************************************************************************/
bool Space::add_character(Character *character)
{
  bool added_character = false;

  if (this->present_character == NULL && this->passable()){
    this->present_character = character;
    added_character = true;
  }

  return added_character;
}


/*************************************************************************
 * Function: delete_character
 * Description: removes a character from the space
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the character will have been removed from the space
 * Returns: bool - true if the character was removed
 ************************************************************************/
bool Space::delete_character()
{
  bool deleted = false;

  if ( this->present_character != NULL ) {
    this->present_character = NULL;
    deleted = true;
  }

  return deleted;
}


/*************************************************************************
 * Function: remove_character
 * Description: removes a character from the space
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the character will have been removed
 * Returns: Character * - the character removed
 ************************************************************************/
Character * Space::remove_character()
{
  Character *character = this->present_character;
  this->present_character = NULL;
  return character;
}


        ////////////////////////////////////////////////////////
       //                   OpenSpace                        //
      ////////////////////////////////////////////////////////

/*************************************************************************
 * Function: passable
 * Description: returns whether the space is passable
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: bool - true if the space is passable
 ************************************************************************/
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

/*************************************************************************
 * Function: remove_item
 * Description: removes an item
 * Parameters: item_ID - the item ID of the item to remove
 * Pre-conditions: none
 * Post-conditions: the item, if it was found, is removed.
 * Returns: Item* - the item removed
 ************************************************************************/
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

/*************************************************************************
 * Function: get_render_char
 * Description: gets the render character of the space
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: char - the character to render for the space
 ************************************************************************/
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

/*************************************************************************
 * Function: open
 * Description: opens the door 
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the door will have been opened
 * Returns: bool - the door was opened (and was not already open)
 ************************************************************************/
bool Door::open()
{
  bool was_open = this->is_open;
  this->is_open = true;
  return (was_open != this->is_open);
}


/*************************************************************************
 * Function: close
 * Description: closes the door
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the door may have been closed
 * Returns: bool - the door was closed (and was not already closed)
 ************************************************************************/
bool Door::close()
{
  bool was_open = this->is_open;
  
  if (this->is_open) {
    this->is_open = false;
  }

  return (was_open != this->is_open);
}


/*************************************************************************
 * Function: get_render_char
 * Description: returns the character to render for the door space
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: char - the character to render
 ************************************************************************/
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
       //                    Secret Door                     //
      ////////////////////////////////////////////////////////

/*************************************************************************
 * Function: open
 * Description: opens the secret door
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: bool - true if the door was opened (and was not already open)
 ************************************************************************/
bool SecretDoor::open()
{
  bool was_open = this->is_open;
  this->is_open = true;
  return (this->is_open != was_open);
}

/*************************************************************************
 * Function: get_render_char
 * Description: returns the character to render for the secret door
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: char - the character to render
 ************************************************************************/
char SecretDoor::get_render_char() const
{
  char c;

  if ( this->present_character != NULL ) {
    c = this->present_character->get_render_char();
  } else if ( this->is_open ){
    c = EMPTY_SPACE_C;
  } else {
    c = WALL_C;
  }

  return c;
}
