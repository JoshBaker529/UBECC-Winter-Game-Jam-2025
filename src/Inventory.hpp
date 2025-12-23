// This file holds the structure for the inventory menu
// As of right now, it does not save what the inventory is

#include "Item.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Keyboard.hpp"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// Defines, because magic numbers are bad :D
////////////////////////////////////////////////////////////////////////////////

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
#define TRANSPARENT 0x00000000

// Textures!
#define TEXTURE_WIDTH 100.f
#define TEXTURE_HEIGHT 100.f

// Insert/Remove
enum AddRemove { ADD, REMOVE };

////////////////////////////////////////////////////////////////////////////////
// Class Definition
////////////////////////////////////////////////////////////////////////////////

class Inventory {
private:
  sf::Vector2f start;
  sf::VertexArray array;
  bool open;
  std::array<Item, SLOTS> inventory;
  std::array<bool, SLOTS> slot_filled;

  void changeInventorySlot(int, AddRemove, Item = Item());
  int VertexArrayIndexFromPosition(int);

public:
  Inventory(sf::Vector2f);
  ~Inventory();
  bool isOpen();
  void toggleOpen();
  sf::VertexArray getArray();
  void insertItem(int, Item);
  void removeItem(int);

  void draw(sf::RenderWindow &);
};

////////////////////////////////////////////////////////////////////////////////
// Private Methods
////////////////////////////////////////////////////////////////////////////////

inline void Inventory::changeInventorySlot(int pos, AddRemove ar, Item item) {

  int starting_index = VertexArrayIndexFromPosition(pos);

  // For now, just changing the color
  // Will need to update texture coords later
  switch (ar) {
  case ADD:
    array[starting_index + 0].color = sf::Color(SLOT_COLOR + 0xE0101000);
    array[starting_index + 1].color = sf::Color(SLOT_COLOR + 0xE0101000);
    array[starting_index + 2].color = sf::Color(SLOT_COLOR + 0xE0101000);
    array[starting_index + 3].color = sf::Color(SLOT_COLOR + 0xE0101000);
    array[starting_index + 4].color = sf::Color(SLOT_COLOR + 0xE0101000);
    array[starting_index + 5].color = sf::Color(SLOT_COLOR + 0xE0101000);
    break;
  case REMOVE:
    array[starting_index + 0].color = sf::Color(TRANSPARENT);
    array[starting_index + 1].color = sf::Color(TRANSPARENT);
    array[starting_index + 2].color = sf::Color(TRANSPARENT);
    array[starting_index + 3].color = sf::Color(TRANSPARENT);
    array[starting_index + 4].color = sf::Color(TRANSPARENT);
    array[starting_index + 5].color = sf::Color(TRANSPARENT);
    break;
  }
}

inline int Inventory::VertexArrayIndexFromPosition(int pos) {
  int index_start = (SLOTS + 1) * 6;
  int index = pos * 6 + index_start;
  return index;
}

////////////////////////////////////////////////////////////////////////////////
// Public Methods
////////////////////////////////////////////////////////////////////////////////

inline Inventory::Inventory(sf::Vector2f vec) {
  start = vec;

  for (int i = 0; i < SLOTS; i++) {
    slot_filled[i] = false;
  }

  // NOTE:
  // Explanation of vertexCount:
  // SLOTS  -- number of inventory slots
  // * 2    -- One for the base layer, one for the items layer
  // + 1    -- Background rectangle
  array = sf::VertexArray(sf::PrimitiveType::Triangles,
                          ((SLOTS * 2) + 1) * VERTICES_PER_SQUARE);

  array[0].position = sf::Vector2f{-350.f, -300.f};
  array[1].position = sf::Vector2f{350.f, -300.f};
  array[2].position = sf::Vector2f{350.f, 0.f};
  array[3].position = sf::Vector2f{-350.f, -300.f};
  array[4].position = sf::Vector2f{-350.f, 0.f};
  array[5].position = sf::Vector2f{350.f, 0.f};

  array[0].color = sf::Color(BACKGROUND_COLOR);
  array[1].color = sf::Color(BACKGROUND_COLOR);
  array[2].color = sf::Color(BACKGROUND_COLOR);
  array[3].color = sf::Color(BACKGROUND_COLOR);
  array[4].color = sf::Color(BACKGROUND_COLOR);
  array[5].color = sf::Color(BACKGROUND_COLOR);

  float x = start.x;
  float y = start.y;

  float slot_width = WIDTH / COLUMNS;
  float slot_height = HEIGHT / ROWS;

  float border_width = slot_width * .05;
  float boarder_height = slot_height * .05;

  // Base Layer
  for (int slot = 0; slot < 21; slot++) {
    if (slot % COLUMNS == 0 && slot != 0) {
      x = start.x;
      y = y + slot_height;
    }

    int index = slot * 6 + 6;
    sf::Vector2f UpLeft(x + border_width, y + boarder_height),
        UpRight(x + slot_width - border_width, y + boarder_height),
        DownLeft(x + border_width, y + slot_height - boarder_height),
        DownRight(x + slot_width - border_width,
                  y + slot_width - boarder_height);

    array[index + 0].position = UpLeft;
    array[index + 1].position = UpRight;
    array[index + 2].position = DownRight;
    array[index + 3].position = DownRight;
    array[index + 4].position = DownLeft;
    array[index + 5].position = UpLeft;

    array[index + 0].color = sf::Color(SLOT_COLOR);
    array[index + 1].color = sf::Color(SLOT_COLOR);
    array[index + 2].color = sf::Color(SLOT_COLOR);
    array[index + 3].color = sf::Color(SLOT_COLOR);
    array[index + 4].color = sf::Color(SLOT_COLOR);
    array[index + 5].color = sf::Color(SLOT_COLOR);

    x += slot_width;
  }

  x = start.x;
  y = start.y;
  // Item Layer
  for (int slot = 0; slot < 21; slot++) {
    if (slot % COLUMNS == 0 && slot != 0) {
      x = start.x;
      y = y + slot_height;
    }

    int index_start = (SLOTS + 1) * 6;
    int index = slot * 6 + index_start;
    sf::Vector2f UpLeft(x + border_width, y + boarder_height),
        UpRight(x + slot_width - border_width, y + boarder_height),
        DownLeft(x + border_width, y + slot_height - boarder_height),
        DownRight(x + slot_width - border_width,
                  y + slot_width - boarder_height);

    array[index + 0].position = UpLeft;
    array[index + 1].position = UpRight;
    array[index + 2].position = DownRight;
    array[index + 3].position = DownRight;
    array[index + 4].position = DownLeft;
    array[index + 5].position = UpLeft;

    if (slot_filled[slot]) {
      sf::Vector2f item_texture = inventory[slot].getTexturePosition();
      sf::Vector2f TextureUpLeft(item_texture),
          TextureUpRight(item_texture.x + TEXTURE_WIDTH, item_texture.y),
          TextureDownLeft(item_texture.x, item_texture.y + TEXTURE_HEIGHT),
          TextureDownRight(item_texture.x + TEXTURE_WIDTH,
                           item_texture.y + TEXTURE_HEIGHT);

      array[index + 0].texCoords = TextureUpLeft;
      array[index + 1].texCoords = TextureUpRight;
      array[index + 2].texCoords = TextureDownRight;
      array[index + 3].texCoords = TextureDownRight;
      array[index + 4].texCoords = TextureDownLeft;
      array[index + 5].texCoords = TextureUpLeft;
      array[index + 0].color = sf::Color(SLOT_COLOR + 0x30101000);
      array[index + 1].color = sf::Color(SLOT_COLOR + 0x30101000);
      array[index + 2].color = sf::Color(SLOT_COLOR + 0x30101000);
      array[index + 3].color = sf::Color(SLOT_COLOR + 0x30101000);
      array[index + 4].color = sf::Color(SLOT_COLOR + 0x30101000);
      array[index + 5].color = sf::Color(SLOT_COLOR + 0x30101000);
    } else {
      array[index + 0].color = sf::Color(TRANSPARENT);
      array[index + 1].color = sf::Color(TRANSPARENT);
      array[index + 2].color = sf::Color(TRANSPARENT);
      array[index + 3].color = sf::Color(TRANSPARENT);
      array[index + 4].color = sf::Color(TRANSPARENT);
      array[index + 5].color = sf::Color(TRANSPARENT);
    }
    x += slot_width;
  }
}

inline Inventory::~Inventory() {
  // idk, I'm not using pointers ..
}

inline bool Inventory::isOpen() { return open; }

inline void Inventory::toggleOpen() { open ^= 1; }

inline sf::VertexArray Inventory::getArray() { return array; }

inline void Inventory::draw(sf::RenderWindow &window) {
  if (open) {
    window.draw(array);
  }
}

inline void Inventory::insertItem(int pos, Item item) {
  if (pos < 0 || pos > SLOTS) {
    return;
  }
  slot_filled[pos] = true;
  inventory[pos] = item;
  changeInventorySlot(pos, ADD, item);
}

inline void Inventory::removeItem(int pos) {
  if (pos < 0 || pos > SLOTS) {
    return;
  }
  slot_filled[pos] = false;
  inventory[pos] = Item();
  changeInventorySlot(pos, REMOVE);
}
