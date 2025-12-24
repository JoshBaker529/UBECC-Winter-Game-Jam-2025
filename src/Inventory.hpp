// This file holds the structure for the inventory menu
// As of right now, it does not save what the inventory is

#include "Controls.hpp"
#include "Item.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>

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

// The index of the floating item
#define FLOATING_INDEX (VertexArrayIndexFromSlot(SLOTS - 1) + 6)

// Variables to hold where the starting positions are for moving an item
static sf::Vector2i mouse_start;
static sf::Vector2f slot_start_positions[VERTICES_PER_SQUARE];

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
  bool moving;
  Item floating_item;

  int VertexArrayIndexFromSlot(int);
  int BackgroundVertexArrayIndexFromSlot(int);
  int getFirstFreeSlot();
  int getVertexFromPosition(sf::Vector2i);
  void updateTextures(int);

public:
  Inventory(sf::Vector2f);
  ~Inventory();
  bool isOpen();
  void toggleOpen();
  sf::VertexArray getArray();
  void insertItem(int, Item);
  void removeItem(int);
  void addItem(Item);

  void draw(sf::RenderWindow &);
};

////////////////////////////////////////////////////////////////////////////////
// Private Methods
////////////////////////////////////////////////////////////////////////////////

// Translates an array position in inventory or slot_filled
// into the starting vertex in array for the item
inline int Inventory::VertexArrayIndexFromSlot(int pos) {
  int index_start = (SLOTS + 1) * 6;
  int index = pos * 6 + index_start;
  return index;
}

// Translates an array position in inventory or slot_filled
// into the starting vertex in array for the slot
inline int Inventory::BackgroundVertexArrayIndexFromSlot(int pos) {
  int index = (pos * 6) + 6;
  return index;
}

// Retrieves the index of first slot that doesn't have an item
// Returns -1 if no empty slot
inline int Inventory::getFirstFreeSlot() {
  for (int i = 0; i < SLOTS; i++) {
    if (!slot_filled[i]) {
      return i;
    }
  }
  return -1;
}

// Given an x,y position, determine if it's within a slot and if so, return the
// index of that slot. Returns -1 if positoin does not fall within a slot
inline int Inventory::getVertexFromPosition(sf::Vector2i pos) {
  // Check overall bounds
  int first = BackgroundVertexArrayIndexFromSlot(0);
  int end = BackgroundVertexArrayIndexFromSlot(SLOTS - 1) + 2;
  sf::Vector2f start_slot = array[first].position;
  sf::Vector2f end_slot = array[end].position;

  if (pos.x < start_slot.x || pos.x > end_slot.x || pos.y < start_slot.y ||
      pos.y > end_slot.y) {
    return -1;
  }

  // Find x
  int x;
  for (x = 0; x < COLUMNS; x++) {
    int UpLeft = BackgroundVertexArrayIndexFromSlot(x);
    int UpRight = UpLeft + 1;

    // We've passed the mouse position
    if (pos.x < array[UpLeft].position.x)
      return -1;

    if (pos.x >= array[UpLeft].position.x && pos.x <= array[UpRight].position.x)
      break;
  }

  // Find y
  int y;
  for (y = 0; y < ROWS; y++) {
    int index = y * COLUMNS + x;
    int UpLeft = BackgroundVertexArrayIndexFromSlot(index);
    int DownLeft = UpLeft + 2;

    // We've passed the mouse position
    if (pos.y < array[UpLeft].position.y)
      return -1;

    if (pos.y >= array[UpLeft].position.y &&
        pos.y <= array[DownLeft].position.y)
      break;
  }

  int ret = y * COLUMNS + x;
  return ret;
}

inline void Inventory::updateTextures(int index) {
  int vector_index;
  bool tex;
  Item item;
  if (index == FLOATING_INDEX) {
    vector_index = FLOATING_INDEX;
    tex = moving;
    item = floating_item;
  } else {
    vector_index = VertexArrayIndexFromSlot(index);
    tex = slot_filled[index];
    item = inventory[index];
  }

  if (tex) {

    sf::Vector2f texStart = item.getTexturePosition();

    sf::Vector2f UpLeft = texStart,
                 UpRight(texStart.x + TEXTURE_WIDTH, texStart.y),
                 DownLeft(texStart.x, texStart.y + TEXTURE_HEIGHT),
                 DownRight(texStart.x + TEXTURE_WIDTH,
                           texStart.y + TEXTURE_HEIGHT);

    /*
    array[vector_index + 0].texCoords = UpLeft;
    array[vector_index + 1].texCoords = UpRight;
    array[vector_index + 2].texCoords = DownRight;
    array[vector_index + 3].texCoords = DownRight;
    array[vector_index + 4].texCoords = DownLeft;
    array[vector_index + 5].texCoords = UpLeft;
    */
    /*
    array[vector_index + 0].color = sf::Color(SLOT_COLOR + 0xE0101000);
    array[vector_index + 1].color = sf::Color(SLOT_COLOR + 0xE0101000);
    array[vector_index + 2].color = sf::Color(SLOT_COLOR + 0xE0101000);
    array[vector_index + 3].color = sf::Color(SLOT_COLOR + 0xE0101000);
    array[vector_index + 4].color = sf::Color(SLOT_COLOR + 0xE0101000);
    array[vector_index + 5].color = sf::Color(SLOT_COLOR + 0xE0101000);
    */

    array[vector_index + 0].color = sf::Color(item.temp_color);
    array[vector_index + 1].color = sf::Color(item.temp_color);
    array[vector_index + 2].color = sf::Color(item.temp_color);
    array[vector_index + 3].color = sf::Color(item.temp_color);
    array[vector_index + 4].color = sf::Color(item.temp_color);
    array[vector_index + 5].color = sf::Color(item.temp_color);
  } else {
    array[vector_index + 0].color = sf::Color(TRANSPARENT);
    array[vector_index + 1].color = sf::Color(TRANSPARENT);
    array[vector_index + 2].color = sf::Color(TRANSPARENT);
    array[vector_index + 3].color = sf::Color(TRANSPARENT);
    array[vector_index + 4].color = sf::Color(TRANSPARENT);
    array[vector_index + 5].color = sf::Color(TRANSPARENT);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Public Methods
////////////////////////////////////////////////////////////////////////////////

inline Inventory::Inventory(sf::Vector2f vec) {
  start = vec;

  for (int i = 0; i < SLOTS; i++) {
    slot_filled[i] = false;
  }
  moving = false;

  // NOTE:
  // Explanation of vertexCount:
  // SLOTS  -- number of inventory slots
  // * 2    -- One for the base layer, one for the items layer
  // + 1    -- Background rectangle
  // + 1    -- Floating inventory slot
  array = sf::VertexArray(sf::PrimitiveType::Triangles,
                          ((SLOTS * 2) + 2) * VERTICES_PER_SQUARE);

  sf::Vector2f UpLeft(start.x, start.y), UpRight(start.x + WIDTH, start.y),
      DownLeft(start.x, start.y + HEIGHT),
      DownRight(start.x + WIDTH, start.y + HEIGHT);

  array[0].position = UpLeft;
  array[1].position = UpRight;
  array[2].position = DownRight;
  array[3].position = DownRight;
  array[4].position = DownLeft;
  array[5].position = UpLeft;

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

// returns the bool if the inventory is open
inline bool Inventory::isOpen() { return open; }

// Toggles the displaying of the inventory screen
inline void Inventory::toggleOpen() { open ^= 1; }

// Gets the vertex array
inline sf::VertexArray Inventory::getArray() { return array; }

// Draws the inventory
inline void Inventory::draw(sf::RenderWindow &window) {
  sf::Vector2i mousePos = Controls::mousePosition();
  if (open) {
    if (Controls::tapped(sf::Mouse::Button::Left)) {
      int index = getVertexFromPosition(mousePos);
      if (!moving) { // Not currently moving
        if (slot_filled[index]) {
          moving = true;
          mouse_start = mousePos;
          if (index != -1) {
            floating_item = inventory[index];
            removeItem(index);
            updateTextures(index);
            updateTextures(FLOATING_INDEX);
            /*
            int vertex_index = VertexArrayIndexFromSlot(index);
            for (int i = 0; i < VERTICES_PER_SQUARE; i++) {
              // slot_start_positions[i] = array[vertex_index + i].position;
            }
            */
          }
        }
      } else { // Currently moving an item
        // swap items
        // replace held item

        bool temp_filled = slot_filled[index];
        Item temp_item = inventory[index];
        moving = slot_filled[index];

        slot_filled[index] = true;
        inventory[index] = floating_item;

        if (temp_filled) {
          floating_item = temp_item;
        } else {
          floating_item = Item();
        }
        updateTextures(FLOATING_INDEX);
        updateTextures(index);
      }
    }

    if (moving) {
      float x_offset = (WIDTH / COLUMNS / 2);
      float y_offset = (HEIGHT / ROWS / 2);

      sf::Vector2f UpLeft(mousePos.x - x_offset, mousePos.y - y_offset),
          UpRight(mousePos.x + x_offset, mousePos.y - y_offset),
          DownLeft(mousePos.x - x_offset, mousePos.y + y_offset),
          DownRight(mousePos.x + x_offset, mousePos.y + y_offset);

      array[FLOATING_INDEX + 0].position = UpLeft;
      array[FLOATING_INDEX + 1].position = UpRight;
      array[FLOATING_INDEX + 2].position = DownRight;
      array[FLOATING_INDEX + 3].position = DownRight;
      array[FLOATING_INDEX + 4].position = DownLeft;
      array[FLOATING_INDEX + 5].position = UpLeft;
    }

    window.draw(array);
  }
}

// Insert item at position
inline void Inventory::insertItem(int pos, Item item) {
  if (pos < 0 || pos > SLOTS) {
    return;
  }
  slot_filled[pos] = true;
  inventory[pos] = item;
  updateTextures(pos);
}

// Remove item at position
inline void Inventory::removeItem(int pos) {
  if (pos < 0 || pos > SLOTS) {
    return;
  }
  slot_filled[pos] = false;
  inventory[pos] = Item();
  updateTextures(pos);
}

// Generic add item to inventory
// Equivalent to InsertItem(int, Item)
// Where int is the first open slot;
inline void Inventory::addItem(Item item) {
  int pos = getFirstFreeSlot();
  if (pos < 0) {
    return;
  }
  insertItem(pos, item);
}
