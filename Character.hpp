#ifndef CHARACTER_HPP
#define CHARACTER_HPP
#include <map>
#include <string>
#include <vector>
#include "Coord.hpp"

class Item;
class Die;

enum ability { STR, DEX, CON, INT, WIS, CHA };

struct mob_data {
  std::string id;
  std::string name;
  char render_char;
  int ac,
      hp,
      die_n,
      die_s,
      die_m,
      b_atk;
  double cr;

  std::vector<std::pair<std::string, int>> loot;
};

struct attack_data {
  int attack_roll;
  int damage_roll;
};

const char PC_RENDER_C = '@';
const Coord STARTING_COORD(4,4);

class Character {
  protected:
    std::string name;
    char render_char;
    Coord coord;
    std::map<Item *, int> inventory;
    int hp;
    int max_hp;
    int b_atk;

  public:
    Character(std::string name, char render_char, Coord coord);
    virtual ~Character() {}

    char get_render_char() { return this->render_char; }
    void set_coord(int x, int y) { this->coord = Coord(x,y); }
    void set_coord(Coord coord) { this->coord = coord; }
    Coord get_coord() { return this->coord; }
    bool has(Item *item)  { return (this->inventory.find(item) != this->inventory.end()); } 
    int item_count(Item *item) { return (this->inventory.find(item)->second); }
    void add_item(Item *);
    bool remove_item(Item *);

    void set_name(std::string name) { this->name = name; }
    std::string get_name() { return this-> name; }
    int get_hp() { return this->hp; }
    int get_max_hp() { return this->max_hp; };
    bool is_dead() { return this->hp <= 0; }

    virtual attack_data attack() = 0;
    virtual bool defend(attack_data) = 0;

    std::map<Item *, int> *get_inventory() { return &(this->inventory); }
};

class Player : public Character {
  private:
    std::map<ability, int> ability_score;    
    Item *equipped_weapon;
    Item *equipped_armor;
    int experience;
    int level;

  public:   
    Player(std::string name = "");

    int get_ability_mod(ability ab) { return (this->ability_score[ab] - 10) / 2;  }
    int get_ability(ability ab) { return this->ability_score[ab]; }
    void inc_ability(ability ab);

    double carry_weight();
    double max_carry();
    bool encumbered();
    void equip_item(Item * item);
    Item *get_weapon() { return this->equipped_weapon; }
    Item *get_armor() { return this->equipped_armor; }
    bool add_experience(int exp);
    
    virtual attack_data attack();
    virtual bool defend(attack_data);
};

class Mob : public Character {
  protected:
    Die *damage_die;

    int ac,
        die_n,
        die_s,
        die_m;
    double cr;
    
  public:
    Mob(mob_data *data, Coord coord);
    ~Mob();

    virtual attack_data attack();
    virtual bool defend(attack_data);
    int get_experience() { return this->cr * 300; }
};

#endif
