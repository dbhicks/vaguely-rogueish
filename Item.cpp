/*************************************************************************
 * Program Filename: Item.cpp
 * Author: David Bacher-Hicks
 * Date: 3 December 2016
 * Description: A class definition file for an item class
 * Input:  none
 * Output: none
 ************************************************************************/

#include "Item.hpp"

/************************************************************************
 * Function: Item
 * Description: constructor
 * Parameters: item_id - the item id (key value)
 *             item_name - the name of the item, human readable
 *             item_description - a brief description of the item
 *             item_weight - weight of the item (lbs)
 *             item_value - value of the item
 *
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: none
 ***********************************************************************/
Item::Item(std::string item_ID,
           std::string item_name,
           std::string item_description,
           double item_weight,
           double item_value)
{
  this->item_ID = item_ID;
  this->item_name = item_name;
  this->item_description = item_description;
  this->item_weight = item_weight;
  this->item_value = item_value;
  this->is_passable = true;
}
