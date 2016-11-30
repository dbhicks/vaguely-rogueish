#include "Character.hpp"
#include "Item.hpp"

void Character::add_item(Item* item)
{
  if (this->inventory.find(item) != this->inventory.end()){
    this->inventory.insert(std::pair<Item*, int>(item, 1));
  } else {
    this->inventory[item]++;
  }
}
