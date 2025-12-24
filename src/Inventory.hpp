// This file holds the structure for the inventory menu
// As of right now, it does not save what the inventory is

#include "Controls.hpp"
#include "Crafting.hpp" // Include Item.hpp and ItemList.hpp

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <iostream>
#include <string>

// TODO:
// Add automatic updating of the crafting list and displaying
// Fully implement the ability to craft items from inventory
// Add CraftingFlags togglables

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
#define TEXT_FILL 0xFFFFFFFF
#define TEXT_OUTLINE_COLOR 0x000000FF
#define TOOLTOP_BACKGROUND_COLOR 0x202080FF

// Text
#define TEXT_OUTLINE_SIZE 0.5f

// Textures!
#define TEXTURE_WIDTH 100.f
#define TEXTURE_HEIGHT 100.f

// Text offsets
#define ONE_DIGIT -10
#define TWO_DIGITS 8

// Tooltip stuff
#define TEXTBOX_BORDER 10
#define TEXTBOX_OFFSET TEXTBOX_BORDER / 2
#define TOOLTIP_OFFSET_LEFT 10
#define TOOLTIP_OFFSET_UP 30

// The index of the floating item
// WARN: Not actually a valid array index!!
#define FLOATING_INDEX (VertexArrayIndexFromSlot(SLOTS - 1) + 6)

// Variables to hold where the starting positions are for moving an item

// The font used
static sf::Font font("res/tuffy.ttf");

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
  sf::VertexArray floating_icon;
  Item floating_item;
  ItemQuantities item_quantities;
  Crafting crafting;
  CraftingFlags crafting_flags;

  int VertexArrayIndexFromSlot(int);
  int BackgroundVertexArrayIndexFromSlot(int);
  int getFirstFreeSlot();
  int getVertexFromPosition(sf::Vector2i);
  void updateTextures(int);
  void initItemQuantities();

public:
  Inventory(sf::Vector2f);
  ~Inventory();
  bool isOpen();
  void toggleOpen();
  sf::VertexArray getArray();
  void insertItem(int, Item);
  void removeItem(int);
  void addItem(Item);
  void setCraftingFlags(CraftingFlags);

  void printCrafting();
  void printInventory();

  void draw(sf::RenderWindow &);
};

////////////////////////////////////////////////////////////////////////////////
// Private Methods
////////////////////////////////////////////////////////////////////////////////

// Translates an array position in inventory or slot_filled
// into the starting vertex in array for the item
int Inventory::VertexArrayIndexFromSlot(int pos) {
  int index_start = (SLOTS + 1) * 6;
  int index = pos * 6 + index_start;
  return index;
}

// Translates an array position in inventory or slot_filled
// into the starting vertex in array for the slot
int Inventory::BackgroundVertexArrayIndexFromSlot(int pos) {
  int index = (pos * 6) + 6;
  return index;
}

// Retrieves the index of first slot that doesn't have an item
// Returns -1 if no empty slot
int Inventory::getFirstFreeSlot() {
  for (int i = 0; i < SLOTS; i++) {
    if (!slot_filled[i]) {
      return i;
    }
  }
  return -1;
}

// Given an x,y position, determine if it's within a slot and if so, return the
// index of that slot. Returns -1 if positoin does not fall within a slot
int Inventory::getVertexFromPosition(sf::Vector2i pos) {
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

// Updates the texCoords for a given index
// Uses floating_item for the reference if index is FLOATING_INDEX
void Inventory::updateTextures(int index) {
  int vector_index = VertexArrayIndexFromSlot(index);
  bool tex;

  vector_index = VertexArrayIndexFromSlot(index);

  if (index == FLOATING_INDEX) {
    if (floating_item.getName() == "")
      tex = false;
    else
      tex = true;
  } else
    tex = slot_filled[index];

  if (tex) {
    // I cannot be bothered to refactor this properly since switching
    // the floating item to a separate VertexArray...
    if (index == FLOATING_INDEX) {
      sf::Vector2f texStart = floating_item.getTexturePosition();

      sf::Vector2f UpLeft = texStart,
                   UpRight(texStart.x + TEXTURE_WIDTH, texStart.y),
                   DownLeft(texStart.x, texStart.y + TEXTURE_HEIGHT),
                   DownRight(texStart.x + TEXTURE_WIDTH,
                             texStart.y + TEXTURE_HEIGHT);

      /*
      floating_icon[vector_index + 0].texCoords = UpLeft;
      floating_icon[vector_index + 1].texCoords = UpRight;
      floating_icon[vector_index + 2].texCoords = DownRight;
      floating_icon[vector_index + 3].texCoords = DownRight;
      floating_icon[vector_index + 4].texCoords = DownLeft;
      floating_icon[vector_index + 5].texCoords = UpLeft;
      */
      /*
      floating_icon[vector_index + 0].color = sf::Color(SLOT_COLOR +
      0xE0101000); floating_icon[vector_index + 1].color = sf::Color(SLOT_COLOR
      + 0xE0101000); floating_icon[vector_index + 2].color =
      sf::Color(SLOT_COLOR + 0xE0101000); floating_icon[vector_index + 3].color
      = sf::Color(SLOT_COLOR + 0xE0101000); floating_icon[vector_index +
      4].color = sf::Color(SLOT_COLOR + 0xE0101000); floating_icon[vector_index
      + 5].color = sf::Color(SLOT_COLOR + 0xE0101000);
      */

      floating_icon[0].color = sf::Color(floating_item.temp_color);
      floating_icon[1].color = sf::Color(floating_item.temp_color);
      floating_icon[2].color = sf::Color(floating_item.temp_color);
      floating_icon[3].color = sf::Color(floating_item.temp_color);
      floating_icon[4].color = sf::Color(floating_item.temp_color);
      floating_icon[5].color = sf::Color(floating_item.temp_color);

      return;
    }
    Item item = inventory[index];
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
    if (index == FLOATING_INDEX) {
      floating_icon[vector_index + 0].color = sf::Color(TRANSPARENT);
      floating_icon[vector_index + 1].color = sf::Color(TRANSPARENT);
      floating_icon[vector_index + 2].color = sf::Color(TRANSPARENT);
      floating_icon[vector_index + 3].color = sf::Color(TRANSPARENT);
      floating_icon[vector_index + 4].color = sf::Color(TRANSPARENT);
      floating_icon[vector_index + 5].color = sf::Color(TRANSPARENT);
      return;
    }
    array[vector_index + 0].color = sf::Color(TRANSPARENT);
    array[vector_index + 1].color = sf::Color(TRANSPARENT);
    array[vector_index + 2].color = sf::Color(TRANSPARENT);
    array[vector_index + 3].color = sf::Color(TRANSPARENT);
    array[vector_index + 4].color = sf::Color(TRANSPARENT);
    array[vector_index + 5].color = sf::Color(TRANSPARENT);
  }
}

// Populates item_quantities with default values
void Inventory::initItemQuantities() {
  for (auto it = ItemList.begin(); it != ItemList.end(); it++) {
    item_quantities[it->first] = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Public Methods
////////////////////////////////////////////////////////////////////////////////

// Constructor
Inventory::Inventory(sf::Vector2f vec) {
  // Don't ask why I have this as a param, and not a define like everything else
  start = vec;

  for (int i = 0; i < SLOTS; i++) {
    slot_filled[i] = false;
  }
  moving = false;
  crafting_flags = NO_FLAG;

  // NOTE:
  // Explanation of vertexCount:
  // SLOTS  -- number of inventory slots
  // * 2    -- One for the base layer, one for the items layer
  // + 1    -- Background rectangle
  array = sf::VertexArray(sf::PrimitiveType::Triangles,
                          ((SLOTS * 2) + 1) * VERTICES_PER_SQUARE);
  floating_icon =
      sf::VertexArray(sf::PrimitiveType::Triangles, VERTICES_PER_SQUARE);

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

// Destructor
Inventory::~Inventory() {
  // idk, I'm not using pointers ..
}

// returns the bool if the inventory is open
bool Inventory::isOpen() { return open; }

// Toggles the displaying of the inventory screen
void Inventory::toggleOpen() { open ^= 1; }

// Gets the vertex array
sf::VertexArray Inventory::getArray() { return array; }

// Draws the inventory
void Inventory::draw(sf::RenderWindow &window) {
  sf::Vector2i mousePos = Controls::mousePosition();
  if (open) {
    // BUG: For some reason this isn't working
    if (Controls::tapped(sf::Mouse::Button::Right)) {
      std::cout << "kjn" << std::endl;
      if (!moving) {
        int index = getVertexFromPosition(mousePos);
        if (slot_filled[index]) {
          moving = true;
          floating_item = inventory[index];
          floating_item.setQuantity(floating_item.getQuantity() / 2);
          inventory[index].addQuantity(-floating_item.getQuantity());
          updateTextures(FLOATING_INDEX);
        }
      }
    } else if (Controls::tapped(sf::Mouse::Button::Left)) {
      int index = getVertexFromPosition(mousePos);
      if (!moving) { // Not currently moving
        if (slot_filled[index]) {
          moving = true;
          if (index != -1) {
            floating_item = inventory[index];
            removeItem(index);
            updateTextures(index);
            updateTextures(FLOATING_INDEX);
          }
        }
      } else { // Currently moving an item

        if (inventory[index].getName() == floating_item.getName()) {
          // items are the same, add to stack
          int empty =
              inventory[index].getMaxStack() - inventory[index].getQuantity();
          if (empty > 0) {
            if (empty > floating_item.getQuantity()) {
              inventory[index].addQuantity(floating_item.getQuantity());
              floating_item = Item();
              moving = false;
            } else {
              inventory[index].addQuantity(empty);
              floating_item.addQuantity(-empty); // lol I guess this works
            }
          }

        } else {
          // Swap items

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
        }
        updateTextures(FLOATING_INDEX);
        updateTextures(index);
      }
    }

    window.draw(array);

    // BUG: For some reason, I get an error when closing since adding this...
    // Just the text sections
    for (int slot = 0; slot < SLOTS; slot++) {
      if (inventory[slot].getQuantity() <= 1)
        continue;
      sf::Vector2f position =
          array[VertexArrayIndexFromSlot(slot) + 2].position;

      sf::Text text(font, std::to_string(inventory[slot].getQuantity()));
      unsigned int size = text.getCharacterSize() + 2;
      position.y -= size;
      // Compensate for either 1 or 2 digits
      if (inventory[slot].getQuantity() / 10 > 0) {
        size += TWO_DIGITS;
      } else {
        size += ONE_DIGIT;
      }
      position.x -= size;

      text.setPosition(position);
      text.setFillColor(sf::Color(TEXT_FILL));
      text.setOutlineColor(sf::Color(TEXT_OUTLINE_COLOR));
      text.setOutlineThickness(TEXT_OUTLINE_SIZE);
      text.setStyle(sf::Text::Style::Bold);

      window.draw(text);
    }

    if (moving) {
      float x_offset = (WIDTH / COLUMNS / 2);
      float y_offset = (HEIGHT / ROWS / 2);

      sf::Vector2f UpLeft(mousePos.x - x_offset, mousePos.y - y_offset),
          UpRight(mousePos.x + x_offset, mousePos.y - y_offset),
          DownLeft(mousePos.x - x_offset, mousePos.y + y_offset),
          DownRight(mousePos.x + x_offset, mousePos.y + y_offset);

      floating_icon[0].position = UpLeft;
      floating_icon[1].position = UpRight;
      floating_icon[2].position = DownRight;
      floating_icon[3].position = DownRight;
      floating_icon[4].position = DownLeft;
      floating_icon[5].position = UpLeft;
      window.draw(floating_icon);

      if (floating_item.getQuantity() > 1) {
        sf::Vector2f position = floating_icon[2].position;

        sf::Text text(font, std::to_string(floating_item.getQuantity()));
        unsigned int size = text.getCharacterSize() + 2;
        position.y -= size;
        // Compensate for either 1 or 2 digits
        if (floating_item.getQuantity() / 10 > 0) {
          size += TWO_DIGITS;
        } else {
          size += ONE_DIGIT;
        }
        position.x -= size;

        text.setPosition(position);
        text.setFillColor(sf::Color(TEXT_FILL));
        text.setOutlineColor(sf::Color(TEXT_OUTLINE_COLOR));
        text.setOutlineThickness(TEXT_OUTLINE_SIZE);
        text.setStyle(sf::Text::Style::Bold);

        window.draw(text);
      }
    } else {
      // Only draw text if not holding item
      int index = getVertexFromPosition(mousePos);
      if (index >= 0 && slot_filled[index]) {
        sf::Text text(font, inventory[index].getTooltip());
        text.setPosition(sf::Vector2f{static_cast<float>(mousePos.x),
                                      static_cast<float>(mousePos.y)});
        text.setFillColor(sf::Color(TEXT_FILL));
        text.setOutlineColor(sf::Color(TEXT_OUTLINE_COLOR));
        text.setOutlineThickness(TEXT_OUTLINE_SIZE);
        text.setScale(sf::Vector2f{0.9, 0.9});
        text.setLineSpacing(0.9f);

        sf::FloatRect fr = text.getGlobalBounds();
        text.move(sf::Vector2f{-(fr.size.x + TOOLTIP_OFFSET_LEFT),
                               -TOOLTIP_OFFSET_UP});
        fr = text.getGlobalBounds();

        sf::RectangleShape rect(fr.size +
                                sf::Vector2f{TEXTBOX_BORDER, TEXTBOX_BORDER});
        rect.setPosition(fr.position -
                         sf::Vector2f{TEXTBOX_OFFSET, TEXTBOX_OFFSET});
        rect.setFillColor(sf::Color(TOOLTOP_BACKGROUND_COLOR));

        window.draw(rect);
        window.draw(text);
      }
    }
  }
}

// Insert item at position
void Inventory::insertItem(int pos, Item item) {
  if (pos < 0 || pos > SLOTS) {
    return;
  }
  item_quantities[item.getName()] += item.getQuantity();
  slot_filled[pos] = true;
  inventory[pos] = item;
  updateTextures(pos);
}

// Remove item at position
void Inventory::removeItem(int pos) {
  if (pos < 0 || pos > SLOTS) {
    return;
  }
  Item item = inventory[pos];
  item_quantities[item.getName()] -= item.getQuantity();
  slot_filled[pos] = false;
  inventory[pos] = Item();
  updateTextures(pos);
}

// Generic add item to inventory
// Will first see if there is already equivalent items with space,
// Adds to them first. Otherwise,
// Equivalent to InsertItem(int, Item)
// Where int is the first open slot;
void Inventory::addItem(Item item) {

  for (auto i = inventory.begin(); i != inventory.end(); i++) {
    if (i->getName() == item.getName()) {
      int empty = i->getMaxStack() - i->getQuantity();
      if (empty > 0) {
        if (empty >= item.getQuantity()) {
          i->addQuantity(item.getQuantity());
          item.setQuantity(0);
          return;
        }
        i->addQuantity(empty);
        item.addQuantity(-empty);
      }
    }
  }

  if (item.getQuantity() > 0) {
    int pos = getFirstFreeSlot();
    if (pos < 0) {
      return;
    }
    insertItem(pos, item);
  }
}

// Sets the current crafting flags - shows what objects we're close to
void Inventory::setCraftingFlags(CraftingFlags flags) {
  crafting_flags = flags;
}

// Debug printing to console the craftable items given current item quantities
void Inventory::printCrafting() {
  auto vec = crafting.getCraftable(item_quantities, crafting_flags);
  std::cout << "Craftable: \n";
  for (auto it = vec.begin(); it != vec.end(); it++) {
    std::cout << "Can craft " << it->getName() << std::endl;
  }
}

// Debug printing to console the contents item_quantities
void Inventory::printInventory() {
  std::cout << "Inventory: \n";
  for (auto it = item_quantities.begin(); it != item_quantities.end(); it++) {
    std::cout << it->first << " x" << it->second << std::endl;
  }
}
