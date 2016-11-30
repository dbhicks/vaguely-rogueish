#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>

class Item{
  private:
    bool is_passable;
    std::string itemID;

  public:
    bool passable() { return this->is_passable; }
    std::string getID() { return this->itemID; }
};

#endif
