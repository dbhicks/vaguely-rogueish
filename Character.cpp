/*************************************************************************
 * Program Filename: Character.cpp
 * Author: David Bacher-Hicks
 * Date: 3 December 2016
 * Description: A class definition file for a character class and its 
 *              subclasses, player and mob (monster or beast). Both the
 *              player character (Player) and enemies (mobs) inherit from
 *              a base Character class.
 *
 * Input: none
 * Output: none
 ************************************************************************/

#include <cstdlib>
#include <typeinfo>
#include "Character.hpp"
#include "Item.hpp"
#include  "Die.hpp"

      ////////////////////////////////////////////////////////////
     //                     Character                          //
    ////////////////////////////////////////////////////////////

/*************************************************************************
 * Function: Character
 * Description: Constructor
 * Parameters: 1) name        - the name of the character
 *             2) render_char - the character rendered by the game 
 *             3) coord       - the location of the character on its floor
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: none
 *************************************************************************/
Character::Character(std::string name, char render_char, Coord coord)
{
  this->name = name;
  this->render_char = render_char;
  this->coord = coord;
}


/*************************************************************************
 * Function: add_item 
 * Description: adds an item to the character's inventory
 * Parameters: item - the item to add
 * Pre-conditions: none
 * Post-conditions: the item will be added tot he player's inventory
 * Returns: none
 *************************************************************************/
void Character::add_item(Item* item)
{
  /*
   *  If the item is new, add a new item entry. If the item is not new, 
   *  increment the count in the inventory map
   */
  if (this->inventory.find(item) == this->inventory.end()){
    this->inventory.insert(std::pair<Item*, int>(item, 1));
  } else {
    ++(this->inventory[item]);
  }
}

/*************************************************************************
 * Function: remove_item
 * Description: removes an item from the character's inventory
 * Parameters: item - the item to remove
 * Pre-conditions: none
 * Post-conditions: the item may have been removed
 * Returns: bool - true if the item was successfully removed
 *************************************************************************/
bool Character::remove_item(Item * item)
{
  bool removed = false;

  /*
   *    If the passed item exists in the inventory and it is the last item,
   *    delete the item from the map. Otherwise, decrement the count in
   *    the inventory map.
   */
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

      ////////////////////////////////////////////////////////////
     //                     Player                             //
    ////////////////////////////////////////////////////////////

/*************************************************************************
 * Function: Player 
 * Description: constructor. Sets initial player character parameters, including
 *              stats, initial HP, level, base attack and experience.
 * Parameters: name - the name of the player
 * Pre-conditions: none
 * Post-conditions: the player will be initialized 
 * Returns: none
 *************************************************************************/
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


/*************************************************************************
 * Function: carry_weight
 * Description: calculates and returns the weight held by the player character
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: double - weight carried
 *************************************************************************/
double Player::carry_weight()
{
  double cw = 0;
  for (auto i = this->inventory.begin(); i != this->inventory.end(); i++){
    cw += i->first->weight() * i->second; 
  }
  return cw;
}


/*************************************************************************
 * Function: max_carry
 * Description: calculates and returns the max carry weight
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: double - the maximum weight the player can carry
 *************************************************************************/
double Player::max_carry()
{
  return this->ability_score[STR] * 10;
}


/*************************************************************************
 * Function: encumbered
 * Description: determines if the character is carrying too much
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: bool - true if the character is encumbered, at which point,
 *          they will not be able to move until they drop some weight.
 *************************************************************************/
bool Player::encumbered()
{
  return this->carry_weight() > this->max_carry();
}


/*************************************************************************
 * Function: attack
 * Description: rolls attack and damage based on current level and equipment
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: an attack_data struct with attack and damage information
 *************************************************************************/
attack_data Player::attack()
{
  attack_data atk;

  atk.attack_roll = (rand() % 20 + 1) + this->b_atk + this->get_ability_mod(STR);
  atk.damage_roll = dynamic_cast<Weapon*>(this->equipped_weapon)->roll_damage() + 
                    this->get_ability_mod(STR);

  return atk;
}


/*************************************************************************
 * Function: defend
 * Description: determines if incoming attacks hit or miss; adjusts hp
 * Parameters: attack_data atk
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: bool - true if the player was hit
 *************************************************************************/
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


/*************************************************************************
 * Function: equip_item
 * Description: sets the player's current equipment (armor or weapon)
 * Parameters: item - the equipment to equip
 * Pre-conditions: none
 * Post-conditions: the player's currently equipped item may have changed
 * Returns: none
 *************************************************************************/
void Player::equip_item(Item *item)
{
  if (typeid(*item) == typeid(Weapon)) {
    this->equipped_weapon = item;
  } else if (typeid(*item) == typeid(Armor)) {
    this->equipped_armor = item;
  }
}


/*************************************************************************
 * Function: add_experience
 * Description: adds experience to the player, leveling if necessary
 * Parameters: exp - the experience to add
 * Pre-conditions: none
 * Post-conditions: the player's experience and possibly their level and 
 *                  associated stats may have been altered.
 * Returns: bool - true if this is a fourth level
 *************************************************************************/
bool Player::add_experience(int exp)
{
  this->experience += exp;
  bool fourth_level = false;

  if ( this->experience >= (this->level * this->level + this->level) * 500 ) {
    this->level++;
    this->max_hp += rand() % 10 + 1;
    this->b_atk++;
    if ((this->level % 4) == 0){
      fourth_level = true;
    }
  }

  return fourth_level;
}


/*************************************************************************
 * Function: inc_ability 
 * Description: increments an ability score
 * Parameters: ab - the ability to increment
 * Pre-conditions: none
 * Post-conditions: the passed ability will have been incremented
 * Returns: none
 *************************************************************************/
void Player::inc_ability(ability ab)
{
  this->ability_score[ab]++;
}


      ////////////////////////////////////////////////////////////
     //                     Mob                                //
    ////////////////////////////////////////////////////////////

/*************************************************************************
 * Function: Mob
 * Description: constructor
 * Parameters: data - the mob_data to use to create the mob
 *             coord - the location of the mob
 *
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: none
 *************************************************************************/
Mob::Mob(mob_data *data, Coord coord) :
  Character(data->name, data->render_char, coord)
{
  this->ac = data->ac;
  this->hp = data->hp;
  this->b_atk = data->b_atk;
  this->cr = data->cr;
  this->damage_die = new Die(data->die_n, data->die_s, data->die_m);
}


/*************************************************************************
 * Function: attack
 * Description: constructs and returns an attack data structure
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: none
 * Returns: an attack data structure
 *************************************************************************/
attack_data Mob::attack()
{
  attack_data atk;
  atk.damage_roll = this->damage_die->roll();
  atk.attack_roll = this->b_atk + (rand() % 20 + 1);
  return atk;
}


/*************************************************************************
 * Function: defend
 * Description: determines hit/miss of incoming attacks; adjusts hp
 * Parameters: atk - the attack to defend against
 * Pre-conditions: none
 * Post-conditions: the hp may have been adjusted on a hit
 * Returns: bool - true if the attack hit
 *************************************************************************/
bool Mob::defend(attack_data atk)
{
  bool hit = false;

  if (atk.attack_roll >= this->ac) {
    hit = true;
    this->hp -= atk.damage_roll;
  }

  return hit;
}


/*************************************************************************
 * Function: ~Mob
 * Description: deallocates the damage die object
 * Parameters: none
 * Pre-conditions: none
 * Post-conditions: the damage_die memory is freed
 * Returns: none
 *************************************************************************/
Mob::~Mob()
{
  delete this->damage_die;
}
