// This class holds the framework for every item in the game

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"
#include <cstdint>

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
  Item(std::string, sf::Vector2f, int, TypeFlags, actionFunction = NULL,
       tooltipFunction = NULL);
  void setConsumableStats(float, float, float);
  void setEquipableStats(float, float);
  void draw(sf::RenderWindow &);
  std::string getTooltip();
  sf::Vector2f getTexturePosition();

  Item();
  Item(Item &&) = default;
  Item(const Item &) = default;
  Item &operator=(Item &&) = default;
  Item &operator=(const Item &) = default;
  ~Item();
};
