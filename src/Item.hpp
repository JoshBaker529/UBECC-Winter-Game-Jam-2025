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

// The two function types
typedef void (*actionFunction)();         // Void return, no params
typedef std::string (*tooltipFunction)(); // String return, no params

// NOTE:
// OKOK So Ian is planning to use a single file for the textures
// So I can switch out the RectangleShape for a VertexArray
// Each four points will need to be associated with the coords of
// a texture on that grid.
// Then I'll need to implement a movement for a single item out of the whole
// VertexArray.

// TODO:
// - Implement a typedef that translates name -> texture.
//    - change the texture type
// - Maybe modify the inventory to be not a menu class but the actual inventory
//      This would help sort out the VectorArray and combine them all into one
//      item
//    - Implement movement for a single item out of a massive VertexArray
//    - Add Array of Item objects or linked list ?? Some container
//    - Add Draw function to InventoryMenu

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
  void setConsumableStats(float, float, float);
  void setEquipableStats(float, float);
  void draw(sf::RenderWindow &);
  std::string getTooltip();
  sf::Vector2f getTexturePosition();
  std::string getName();
  int getQuantity();
  void debugPrint();

  Item();
  Item(Item &&) = default;
  Item(const Item &) = default;
  Item &operator=(Item &&) = default;
  Item &operator=(const Item &) = default;
  ~Item();

  uint32_t temp_color;
};

inline Item::Item(std::string name, sf::Vector2f texture, int quantity,
                  int max_stack, TypeFlags type, actionFunction action,
                  tooltipFunction hover)
    : name(name), texture_position(texture), stack_size(quantity),
      max_stack_size(max_stack), type(type), hp_gained(0), hunger_gained(0),
      warmth_gained(0), defense(0), cold_resist(0), useAction(action),
      tooltipHover(hover) {
  // INFO:
  // Constructor intentionally left blank, everything is in initialization
  // list.
  // See setConsumableStats and setEquipableStats for the stats that are
  // not listed here
  temp_color = 0xEBBBBBFF;
};

inline void Item::setConsumableStats(float hp, float hunger, float warmth) {
  hp_gained = hp;
  hunger_gained = hunger;
  warmth_gained = warmth;
}

inline void Item::setEquipableStats(float armor, float cold) {
  defense = armor;
  cold_resist = cold;
}

inline void Item::draw(sf::RenderWindow &window) {
  sf::RectangleShape rect(sf::Vector2f{50, 50});
  rect.setPosition(sf::Vector2f{70, 70});

  window.draw(rect);
}

inline std::string Item::getTooltip() {
  std::stringstream ss;
  if (tooltipHover) {
    ss << tooltipHover();
    return ss.str();
  }

  ss << name << '\n' << stack_size << " / " << max_stack_size;

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

sf::Vector2f Item::getTexturePosition() { return texture_position; }

std::string Item::getName() { return name; }

int Item::getQuantity() { return stack_size; }

void Item::debugPrint() {
  printf("Name: %s, Texture: %f %f, Color: %x\n", name.c_str(),
         texture_position.x, texture_position.y, temp_color);
}

Item::Item()
    : name(""), texture_position(sf::Vector2f{0, 0}), stack_size(0),
      max_stack_size(0), type(0), hp_gained(0), hunger_gained(0),
      warmth_gained(0), defense(0), cold_resist(0), useAction(0),
      tooltipHover(0) {}

Item::~Item() {
  // intentionally left blank, nothing to destroy
}
