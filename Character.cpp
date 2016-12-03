#include <cstdlib>
#include <typeinfo>
#include "Character.hpp"
#include "Item.hpp"
#include  "Die.hpp"

Character::Character(std::string name, char render_char, Coord coord)
{
  this->name = name;
  this->render_char = render_char;
  this->coord = coord;
}

void Character::add_item(Item* item)
{
  if (this->inventory.find(item) == this->inventory.end()){
    this->inventory.insert(std::pair<Item*, int>(item, 1));
  } else {
    ++(this->inventory[item]);
  }
}

bool Character::remove_item(Item * item)
{
  bool removed = false;

  auto it = this->inventory.find(item);

  if ( it != this->inventory.end() && it->second != 0 ){
    if (it->second == 1){
      this->inventory.erase(it);
    } else {
      it->second--;
    }
    bool removed = true;    
  }

  return removed;
}


Player::Player(std::string name) : Character(name, PC_RENDER_C, STARTING_COORD)
{
  this->ability_score[STR] = 18;
  this->ability_score[DEX] = 18;
  this->ability_score[CON] = 18;
  this->ability_score[INT] = 18;
  this->ability_score[WIS] = 18;
  this->ability_score[CHA] = 18;
  this->max_hp = this->hp = 10;
  this->level = 1;
  this->b_atk = 1;
  this->experience = 0;
}

double Player::carry_weight()
{
  double cw = 0;
  for (auto i = this->inventory.begin(); i != this->inventory.end(); i++){
    cw += i->first->weight() * i->second; 
  }
  return cw;
}

double Player::max_carry()
{
  return this->ability_score[STR] * 10;
}

bool Player::encumbered()
{
  return this->carry_weight() > this->max_carry();
}

attack_data Player::attack()
{
  attack_data atk;

  atk.attack_roll = (rand() % 20 + 1) + this->b_atk + this->get_ability_mod(STR);
  atk.damage_roll = dynamic_cast<Weapon*>(this->equipped_weapon)->roll_damage() + 
                    this->get_ability_mod(STR);

  return atk;
}

bool Player::defend(attack_data atk)
{
  bool hit = false;

  int defense = 10 + dynamic_cast<Armor*>(this->equipped_armor)->get_ac() 
                + this->get_ability_mod(DEX);
  if (defense <= atk.attack_roll) {
    this->hp -= atk.damage_roll;
    hit = true;
  }

  return hit;
}

void Player::equip_item(Item *item)
{
  if (typeid(*item) == typeid(Weapon)) {
    this->equipped_weapon = item;
  } else if (typeid(*item) == typeid(Armor)) {
    this->equipped_armor = item;
  }
}

bool Player::add_experience(int exp)
{
  this->experience += exp;

  if ( this->experience >= (this->level * this->level + this->level) * 500 ) {
    this->level++;
    this->max_hp += rand() % 10 + 1;
    this->b_atk++;
  }

  return ((this->level % 4) == 0);
}

void Player::inc_ability(ability ab)
{
  this->ability_score[ab]++;
}

Mob::Mob(mob_data *data, Coord coord) :
  Character(data->name, data->render_char, coord)
{
  this->ac = data->ac;
  this->hp = data->hp;
  this->b_atk = data->b_atk;
  this->cr = data->cr;
  this->damage_die = new Die(data->die_n, data->die_s, data->die_m);
}

attack_data Mob::attack()
{
  attack_data atk;
  atk.damage_roll = this->damage_die->roll();
  atk.attack_roll = this->b_atk + (rand() % 20 + 1);
  return atk;
}

bool Mob::defend(attack_data atk)
{
  bool hit = false;

  if (atk.attack_roll >= this->ac) {
    hit = true;
    this->hp -= atk.damage_roll;
  }

  return hit;
}

Mob::~Mob()
{
  delete this->damage_die;
}
