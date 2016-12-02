#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>
#include "Die.hpp"

class Item {
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
    virtual ~Item() {}

    bool passable()     { return true; }
    std::string id()    { return this->item_ID; }
    std::string description() { return this->item_description; }
    std::string name()  { return this->item_name; }
    double weight()     { return this->item_weight; }
    double value()      { return this->item_value; }
    virtual void ignore_me() {} 
};

class Weapon : public Item {
  private:
    Die *damage_die;

  public:
    Weapon(std::string item_ID, 
           std::string item_name, 
           std::string item_description, 
           double item_weight,
           double item_value,
           int damage_die_num,
           int damage_die_sides,
           int damage_die_mod) : 
      Item(item_ID, item_name, item_description, item_weight, item_value)
      { this->damage_die = new Die(damage_die_num, damage_die_sides, damage_die_mod); }   
    virtual ~Weapon() { delete this->damage_die; }
    int roll_damage() { return this->damage_die->roll(); }

};

class Armor : public Item {
  private:
    int AC;

  public:
    Armor(std::string item_ID, 
           std::string item_name, 
           std::string item_description, 
           double item_weight,
           double item_value,
           int AC) : 
      Item(item_ID, item_name, item_description, item_weight, item_value)
      { this->AC = AC; }
};

#endif
