#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>

class Item{
  private:
    bool        is_passable;
    std::string item_ID;
    std::string item_name;
    std::string item_description;
    double      item_weight;
    double      item_value;

  public:
    Item(std::string item_ID, 
         std::string item_name, 
         std::string item_description, 
         double item_weight,
         double item_value);

    bool passable()     { return true; }
    std::string id()    { return this->item_ID; }
    std::string description() { return this->item_description; }
    std::string name()  { return this->item_name; }
    double weight()     { return this->item_weight; }
    double value()      { return this->item_value; }
};

#endif
