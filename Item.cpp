#include "Item.hpp"

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
