// This class holds the framework for every item in the game

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include <cstdint>

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include <cstdio>
#include <sstream>
// Typedefs for the possible item types
typedef uint8_t TypeFlags;

// The possible options for items, future-proofed
#define CONSUMABLE 0b1
#define EQUIPABLE 0b10
#define TOOL 0b100
#define BLOCK 0b1000
#define NOT_USED_1 0b10000
#define NOT_USED_2 0b100000
#define NOT_USED_3 0b1000000
#define NOT_USED_4 0b10000000

class Item;

// The two function types
typedef void (*actionFunction)(Item *); // Void return, param of itself
typedef std::string (*tooltipFunction)(
    Item *); // String return, param of itself

class Item {
private:
  // Default stuff an item has
  std::string name;
  // sf::Texture texture;
  sf::Vector2f texture_position;
  int stack_size;
  int max_stack_size;
  TypeFlags type;

  // Consumables
  float hp_gained;
  float hunger_gained;
  float warmth_gained;

  // Equipables
  float defense;
  float cold_resist;

  // Tools

  actionFunction useAction;
  tooltipFunction tooltipHover;

public:
  Item(std::string, sf::Vector2f, int, int, TypeFlags, actionFunction = NULL,
       tooltipFunction = NULL);
  Item(std::string, sf::Vector2f, int, int, TypeFlags, float, float, float,
       float, float, actionFunction = NULL, tooltipFunction = NULL);
  void setConsumableStats(float, float, float);
  void setEquipableStats(float, float);
  void draw(sf::RenderWindow &);
  std::string getTooltip();
  sf::Vector2f getTexturePosition();
  std::string getName();
  int getQuantity();
  int getMaxStack();
  TypeFlags getType();
  float getHpGained();
  float getHungerGained();
  float getWarmthGained();
  float getDefenseGained();
  float getColdResist();
  actionFunction getActionFunction();
  tooltipFunction getTooltipFunction();
  void setQuantity(int);
  void addQuantity(int);

  void debugPrint();

  Item();
  Item(Item &&) = default;
  Item(const Item &) = default;
  Item &operator=(Item &&) = default;
  Item &operator=(const Item &) = default;
  ~Item();

  uint32_t temp_color;
};

// Simple constructor
// You should use the full constructor!
Item::Item(std::string name, sf::Vector2f texture, int quantity, int max_stack,
           TypeFlags type, actionFunction action, tooltipFunction hover)
    : name(name), texture_position(texture), stack_size(quantity),
      max_stack_size(max_stack), type(type), hp_gained(0), hunger_gained(0),
      warmth_gained(0), defense(0), cold_resist(0), useAction(action),
      tooltipHover(hover) {
  temp_color = 0xEBBBBBFF;
};

// Full constructor
// Has a param for every member field
Item::Item(std::string name, sf::Vector2f texture, int quantity, int max_stack,
           TypeFlags type, float hp, float hunger, float warmth, float def,
           float cold, actionFunction action, tooltipFunction hover)
    : name(name), texture_position(texture), stack_size(quantity),
      max_stack_size(max_stack), type(type), hp_gained(hp),
      hunger_gained(hunger), warmth_gained(warmth), defense(def),
      cold_resist(cold), useAction(action), tooltipHover(hover) {
  temp_color = 0xEBBBBBFF;
};

// Sets the consumable stats (hp, hunger, and warmth gained)
void Item::setConsumableStats(float hp, float hunger, float warmth) {
  hp_gained = hp;
  hunger_gained = hunger;
  warmth_gained = warmth;
}

// Sets the equipable stats (defense, cold resistance)
void Item::setEquipableStats(float armor, float cold) {
  defense = armor;
  cold_resist = cold;
}

// Iiiii don't use this anymore, but it's nice to have for debugging
void Item::draw(sf::RenderWindow &window) {
  sf::RectangleShape rect(sf::Vector2f{50, 50});
  rect.setPosition(sf::Vector2f{70, 70});

  window.draw(rect);
}
// Returns the string for the tooltip
// Will use the tooltipHover function if provided,
// Otherwise just provides the stats
std::string Item::getTooltip() {
  std::stringstream ss;
  if (tooltipHover) {
    ss << tooltipHover(this);
    return ss.str();
  }

  ss << name << '\n' << stack_size << "/" << max_stack_size;

  if (type & CONSUMABLE)
    ss << "\nHP: " << hp_gained << "\nFood: " << hunger_gained
       << "\nWarmth: " << warmth_gained;

  if (type & EQUIPABLE)
    ss << "\nDefense: " << defense << "\nCold Resistance: " << cold_resist;

  if (type & TOOL)
    ss << "\nIt's a tool!";

  if (type & BLOCK)
    ss << "\nIt's a block!";

  return ss.str();
}

// Returns the texture position
sf::Vector2f Item::getTexturePosition() { return texture_position; }

// Returns the name
std::string Item::getName() { return name; }

// Returns the current stack size
int Item::getQuantity() { return stack_size; }

// Returns the maximum stack size
int Item::getMaxStack() { return max_stack_size; }

// Gets the type of the item, as a bitstring
TypeFlags Item::getType() { return type; }

// Returns the ammount of HP gained
float Item::getHpGained() { return hp_gained; }

// Returns the ammount of hunger gained
float Item::getHungerGained() { return hunger_gained; }

// Returns the ammount of warmth gained
float Item::getWarmthGained() { return warmth_gained; }

// Returns the amount of defense item gives
float Item::getDefenseGained() { return defense; }

// Returns the amount of cold resist item gives
float Item::getColdResist() { return cold_resist; }

// Returns the pointer to the action function
actionFunction Item::getActionFunction() { return useAction; }

// Returns the pointer to the tooltip function
tooltipFunction Item::getTooltipFunction() { return tooltipHover; }

// Sets the quantity
// No checks, it is unsafe if stack size is important
void Item::setQuantity(int qty) { stack_size = qty; }

// Adds to the quantity
// No checks, it is unsafe if the stack size is important
void Item::addQuantity(int qty) { stack_size += qty; }

// Simple debug print to console
void Item::debugPrint() {
  printf("Name: %s, Texture: %f %f, Color: %x\n", name.c_str(),
         texture_position.x, texture_position.y, temp_color);
}

// Default constructor
// DO NOT USE UNLESS YOU KNOW WHAT YOU'RE DOING!
Item::Item()
    : name(""), texture_position(sf::Vector2f{0, 0}), stack_size(0),
      max_stack_size(0), type(0), hp_gained(0), hunger_gained(0),
      warmth_gained(0), defense(0), cold_resist(0), useAction(0),
      tooltipHover(0) {
  temp_color = 0;
}

// Destructor, kinda useless
Item::~Item() {
  // intentionally left blank, nothing to destroy
}
