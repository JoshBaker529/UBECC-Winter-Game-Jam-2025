// This file holds the structure for the inventory menu
// As of right now, it does not save what the inventory is

#include "Item.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>

// Defines, because magic numbers are bad :D
// Size
#define WIDTH 700.f
#define HEIGHT 300.f

// Number of slots
#define SLOTS 21
#define COLUMNS 7
#define ROWS 3

// .. exactly what it says
#define VERTICES_PER_SQUARE 6

// Colors!
#define BACKGROUND_COLOR 0xF0F0F0FF
#define SLOT_COLOR 0xBBBBBBFF

// Textures!
#define TEXTURE_WIDTH 100.f
#define TEXTURE_HEIGHT 100.f

class Inventory {
private:
  sf::Vector2f start;
  sf::VertexArray array;
  bool open;
  std::array<Item, SLOTS> inventory;
  std::array<bool, SLOTS> slot_filled;

public:
  Inventory(sf::Vector2f);
  ~Inventory();
  bool isOpen();
  void toggleOpen();
  sf::VertexArray getArray();

  void draw(sf::RenderWindow);
};
