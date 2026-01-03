#pragma once
// This file holds the structure for the inventory menu
// As of right now, it does not save what the inventory is
#include "Controls.hpp"
#include "Crafting.hpp" // Include Item.hpp and ItemList.hpp

#include "ItemList.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// TODO:
// Need to work out what is needed for equipment/hotbar
// Need a list of items and recipes

////////////////////////////////////////////////////////////////////////////////
// Defines, because magic numbers are bad :D
////////////////////////////////////////////////////////////////////////////////

// Number of slots
#define COLUMNS 5
#define ROWS 3 // One for hotbar
#define SLOTS COLUMNS *ROWS
#define HOTBAR_SLOTS 5

// Size
#define SLOT_SIZE 75.f
#define HOTBAR_BUFFER 10.f
#define WIDTH (COLUMNS * SLOT_SIZE)
#define HEIGHT (ROWS * SLOT_SIZE)
#define TOTAL_HEIGHT (HEIGHT + HOTBAR_BUFFER)

// .. exactly what it says
#define VERTICES_PER_SQUARE 6
#define EQUIP_VERTEX_INDEX ((((SLOTS * 2) + 1) * VERTICES_PER_SQUARE) + 6)

// Colors!
#define BACKGROUND_COLOR 0xF0F0F0FF
#define SLOT_COLOR 0xBBBBBBFF
#define TRANSPARENT 0x00000000
#define TEXT_FILL 0xFFFFFFFF
#define TEXT_OUTLINE_COLOR 0x000000FF
#define TOOLTOP_BACKGROUND_COLOR 0x202080FF
#define HIGHLIGHT_COLOR 0x7070E0FF

// Text
#define TEXT_OUTLINE_SIZE 0.5f

// Textures!
#define TEXTURE_WIDTH 32.f
#define TEXTURE_HEIGHT 32.f

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
#define FLOATING_INDEX (INT32_MAX)
#define EQUIPMENT_INDEX (-2)

// Crafting constants
#define CRAFTING_ICON_WIDTH 50.f
#define CRAFTING_ICON_HEIGHT 50.f
#define CRAFTING_INGREDIENT_WIDTH 30.f
#define CRAFTING_INGREDIENT_HEIGHT 30.f
#define CRAFTING_RECIPES_SHOWN 10
#define CRAFTING_START (sf::Vector2f{start.x, start.y + HEIGHT + 35.f})
#define CRAFTING_INDEX(i) (i * (crafting.getMaxIngredients() + 1) * 6)

// Crafting flag array positions
#define CRAFTING_FLAG_BENCH_INDEX 0
#define CRAFTING_FLAG_FIRE_INDEX 1
#define CRAFTING_FLAG_EMPTY1_INDEX 2
#define CRAFTING_FLAG_EMPTY2_INDEX 3
#define CRAFTING_FLAG_EMPTY3_INDEX 4
#define CRAFTING_FLAG_EMPTY4_INDEX 5
#define CRAFTING_FLAG_EMPTY5_INDEX 6
#define CRAFTING_FLAG_EMPTY6_INDEX 7

#define DEBUG true

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
  sf::VertexArray hotbar;
  sf::VertexArray floating_icon;
  sf::VertexArray crafting_array;

  std::array<Item, SLOTS> inventory;
  std::array<bool, SLOTS> slot_filled;

  std::array<int, 8> flag_counts;

  sf::Texture item_texture;

  bool open;
  bool moving;

  Item floating_item;
  Item equipment;
  ItemQuantities item_quantities;
  Crafting crafting;
  CraftingFlags crafting_flags;

  int hotbar_position;
  int max_ingredients;

  std::vector<Recipe> craftable_list;
  int crafting_position;

  int VertexArrayIndexFromSlot(int);
  int BackgroundVertexArrayIndexFromSlot(int);
  int getFirstFreeSlot();
  int getVertexFromPosition(sf::Vector2i);
  void updateTextures(int);
  void initItemQuantities();
  void updateCraftableList();
  int getCraftingFromPosition(sf::Vector2i);
  // void DrawCrafting(sf::RenderWindow);

public:
  Inventory();
  ~Inventory();
  bool isOpen();
  void toggleOpen();
  sf::VertexArray getArray();
  void insertItem(int, Item);
  void removeItem(int);
  void removeItem(std::string, int);
  void addItem(Item);
  int getHotbarPosition();
  Item getHotbarItem();
  void decHotbarItem();
  void moveHotbarRight();
  void moveHotbarLeft();
  void setHotbarPosition(int);

  void setCraftingFlags(CraftingFlags);
  void craftingFlagSetFire(bool);

  void printCrafting();
  void printInventory();

  void draw(sf::RenderWindow &);
  void drawCrafting(sf::RenderWindow &);

  // static inline Inventory inventory;
};

// static Inventory inventory(sf::Vector2f{0, 0});
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

  if (pos.x > end_slot.x) {
    if (pos.y > start_slot.y &&
        pos.y < (start_slot.y + SLOT_SIZE - (SLOT_SIZE * .05))) {
      return EQUIPMENT_INDEX;
    }
  }

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
  } else if (index == EQUIPMENT_INDEX) {
    if (equipment.getName() == "") {
      tex = false;
    } else {
      tex = true;
    }
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

      floating_icon[0].texCoords = UpLeft;
      floating_icon[1].texCoords = UpRight;
      floating_icon[2].texCoords = DownRight;
      floating_icon[3].texCoords = DownRight;
      floating_icon[4].texCoords = DownLeft;
      floating_icon[5].texCoords = UpLeft;

      floating_icon[0].color = sf::Color(0xFFFFFFFF);
      floating_icon[1].color = sf::Color(0xFFFFFFFF);
      floating_icon[2].color = sf::Color(0xFFFFFFFF);
      floating_icon[3].color = sf::Color(0xFFFFFFFF);
      floating_icon[4].color = sf::Color(0xFFFFFFFF);
      floating_icon[5].color = sf::Color(0xFFFFFFFF);

      return;
    }
    if (index == EQUIPMENT_INDEX) {
      sf::Vector2f texStart = equipment.getTexturePosition();

      sf::Vector2f UpLeft = texStart,
                   UpRight(texStart.x + TEXTURE_WIDTH, texStart.y),
                   DownLeft(texStart.x, texStart.y + TEXTURE_HEIGHT),
                   DownRight(texStart.x + TEXTURE_WIDTH,
                             texStart.y + TEXTURE_HEIGHT);

      array[EQUIP_VERTEX_INDEX + 12 + 0].texCoords = UpLeft;
      array[EQUIP_VERTEX_INDEX + 12 + 1].texCoords = UpRight;
      array[EQUIP_VERTEX_INDEX + 12 + 2].texCoords = DownRight;
      array[EQUIP_VERTEX_INDEX + 12 + 3].texCoords = DownRight;
      array[EQUIP_VERTEX_INDEX + 12 + 4].texCoords = DownLeft;
      array[EQUIP_VERTEX_INDEX + 12 + 5].texCoords = UpLeft;

      array[EQUIP_VERTEX_INDEX + 12 + 0].color = sf::Color(0xFFFFFFFF);
      array[EQUIP_VERTEX_INDEX + 12 + 1].color = sf::Color(0xFFFFFFFF);
      array[EQUIP_VERTEX_INDEX + 12 + 2].color = sf::Color(0xFFFFFFFF);
      array[EQUIP_VERTEX_INDEX + 12 + 3].color = sf::Color(0xFFFFFFFF);
      array[EQUIP_VERTEX_INDEX + 12 + 4].color = sf::Color(0xFFFFFFFF);
      array[EQUIP_VERTEX_INDEX + 12 + 5].color = sf::Color(0xFFFFFFFF);
      return;
    }
    Item item = inventory[index];
    sf::Vector2f texStart = item.getTexturePosition();

    sf::Vector2f UpLeft = texStart,
                 UpRight(texStart.x + TEXTURE_WIDTH, texStart.y),
                 DownLeft(texStart.x, texStart.y + TEXTURE_HEIGHT),
                 DownRight(texStart.x + TEXTURE_WIDTH,
                           texStart.y + TEXTURE_HEIGHT);

    array[vector_index + 0].texCoords = UpLeft;
    array[vector_index + 1].texCoords = UpRight;
    array[vector_index + 2].texCoords = DownRight;
    array[vector_index + 3].texCoords = DownRight;
    array[vector_index + 4].texCoords = DownLeft;
    array[vector_index + 5].texCoords = UpLeft;

    array[vector_index + 0].color = sf::Color(0xFFFFFFFF);
    array[vector_index + 1].color = sf::Color(0xFFFFFFFF);
    array[vector_index + 2].color = sf::Color(0xFFFFFFFF);
    array[vector_index + 3].color = sf::Color(0xFFFFFFFF);
    array[vector_index + 4].color = sf::Color(0xFFFFFFFF);
    array[vector_index + 5].color = sf::Color(0xFFFFFFFF);

    if (index < HOTBAR_SLOTS) {
      vector_index = (HOTBAR_SLOTS + 2) * VERTICES_PER_SQUARE +
                     index * VERTICES_PER_SQUARE;

      hotbar[vector_index + 0].texCoords = UpLeft;
      hotbar[vector_index + 1].texCoords = UpRight;
      hotbar[vector_index + 2].texCoords = DownRight;
      hotbar[vector_index + 3].texCoords = DownRight;
      hotbar[vector_index + 4].texCoords = DownLeft;
      hotbar[vector_index + 5].texCoords = UpLeft;
      hotbar[vector_index + 0].color = sf::Color(0xFFFFFFFF);
      hotbar[vector_index + 1].color = sf::Color(0xFFFFFFFF);
      hotbar[vector_index + 2].color = sf::Color(0xFFFFFFFF);
      hotbar[vector_index + 3].color = sf::Color(0xFFFFFFFF);
      hotbar[vector_index + 4].color = sf::Color(0xFFFFFFFF);
      hotbar[vector_index + 5].color = sf::Color(0xFFFFFFFF);
    }

    return;
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
    if (index == EQUIPMENT_INDEX) {

      array[EQUIP_VERTEX_INDEX + 12 + 0].color = sf::Color(TRANSPARENT);
      array[EQUIP_VERTEX_INDEX + 12 + 1].color = sf::Color(TRANSPARENT);
      array[EQUIP_VERTEX_INDEX + 12 + 2].color = sf::Color(TRANSPARENT);
      array[EQUIP_VERTEX_INDEX + 12 + 3].color = sf::Color(TRANSPARENT);
      array[EQUIP_VERTEX_INDEX + 12 + 4].color = sf::Color(TRANSPARENT);
      array[EQUIP_VERTEX_INDEX + 12 + 5].color = sf::Color(TRANSPARENT);
      return;
    }
    array[vector_index + 0].color = sf::Color(TRANSPARENT);
    array[vector_index + 1].color = sf::Color(TRANSPARENT);
    array[vector_index + 2].color = sf::Color(TRANSPARENT);
    array[vector_index + 3].color = sf::Color(TRANSPARENT);
    array[vector_index + 4].color = sf::Color(TRANSPARENT);
    array[vector_index + 5].color = sf::Color(TRANSPARENT);
    if (index < HOTBAR_SLOTS) {
      vector_index = (HOTBAR_SLOTS + 2) * VERTICES_PER_SQUARE +
                     index * VERTICES_PER_SQUARE;

      hotbar[vector_index + 0].color = sf::Color(TRANSPARENT);
      hotbar[vector_index + 1].color = sf::Color(TRANSPARENT);
      hotbar[vector_index + 2].color = sf::Color(TRANSPARENT);
      hotbar[vector_index + 3].color = sf::Color(TRANSPARENT);
      hotbar[vector_index + 4].color = sf::Color(TRANSPARENT);
      hotbar[vector_index + 5].color = sf::Color(TRANSPARENT);
    }
  }
}

// Populates item_quantities with default values
void Inventory::initItemQuantities() {
  for (auto it = ItemList.begin(); it != ItemList.end(); it++) {
    item_quantities[it->first] = 0;
  }
}

void Inventory::updateCraftableList() {
  crafting.calclulateMaxIngredients();
  if (max_ingredients < crafting.getMaxIngredients()) {
    max_ingredients = crafting.getMaxIngredients();

    crafting_array = sf::VertexArray(
        sf::PrimitiveType::Triangles,
        (CRAFTING_RECIPES_SHOWN * (crafting.getMaxIngredients() + 1)) *
            VERTICES_PER_SQUARE);

    float start_x = CRAFTING_START.x;
    float start_y = CRAFTING_START.y;

    for (int recipe = 0; recipe < CRAFTING_RECIPES_SHOWN; recipe++) {
      float x = start_x;
      float y = start_y;
      float slot_width = CRAFTING_ICON_WIDTH;
      float slot_height = CRAFTING_ICON_HEIGHT;

      float border_width = slot_width * .05;
      float border_height = slot_height * .05;

      sf::Vector2f UpLeft(x + border_width, y + border_height),
          UpRight(x + slot_width - border_width, y + border_height),
          DownLeft(x + border_width, y + slot_height - border_height),
          DownRight(x + slot_width - border_width,
                    y + slot_width - border_height);

      // Just trust me, this is correct
      int index =
          (recipe * (VERTICES_PER_SQUARE * (crafting.getMaxIngredients() + 1)));

      crafting_array[index + 0].position = UpLeft;
      crafting_array[index + 1].position = UpRight;
      crafting_array[index + 2].position = DownRight;
      crafting_array[index + 3].position = DownRight;
      crafting_array[index + 4].position = DownLeft;
      crafting_array[index + 5].position = UpLeft;

      crafting_array[index + 0].color = sf::Color(TRANSPARENT);
      crafting_array[index + 1].color = sf::Color(TRANSPARENT);
      crafting_array[index + 2].color = sf::Color(TRANSPARENT);
      crafting_array[index + 3].color = sf::Color(TRANSPARENT);
      crafting_array[index + 4].color = sf::Color(TRANSPARENT);
      crafting_array[index + 5].color = sf::Color(TRANSPARENT);

      // Prepare for the smaller icons
      border_width = CRAFTING_INGREDIENT_WIDTH * .05;
      border_height = CRAFTING_INGREDIENT_HEIGHT * .05;

      UpLeft = sf::Vector2f(
          start_x + CRAFTING_ICON_WIDTH,
          start_y + ((CRAFTING_ICON_HEIGHT - CRAFTING_INGREDIENT_HEIGHT) / 2) +
              border_height);
      UpRight =
          UpLeft + sf::Vector2f{CRAFTING_INGREDIENT_WIDTH - border_width, 0};
      DownLeft =
          UpLeft + sf::Vector2f{0, CRAFTING_INGREDIENT_HEIGHT - border_height};
      DownRight =
          DownLeft + sf::Vector2f{CRAFTING_INGREDIENT_WIDTH - border_width, 0};

      for (int ingredient = 0; ingredient < crafting.getMaxIngredients();
           ingredient++) {
        index += 6;

        crafting_array[index + 0].position = UpLeft;
        crafting_array[index + 1].position = UpRight;
        crafting_array[index + 2].position = DownRight;
        crafting_array[index + 3].position = DownRight;
        crafting_array[index + 4].position = DownLeft;
        crafting_array[index + 5].position = UpLeft;

        crafting_array[index + 0].color = sf::Color(TRANSPARENT);
        crafting_array[index + 1].color = sf::Color(TRANSPARENT);
        crafting_array[index + 2].color = sf::Color(TRANSPARENT);
        crafting_array[index + 3].color = sf::Color(TRANSPARENT);
        crafting_array[index + 4].color = sf::Color(TRANSPARENT);
        crafting_array[index + 5].color = sf::Color(TRANSPARENT);

        UpLeft += sf::Vector2f{CRAFTING_INGREDIENT_WIDTH, 0};
        UpRight += sf::Vector2f{CRAFTING_INGREDIENT_WIDTH, 0};
        DownLeft += sf::Vector2f{CRAFTING_INGREDIENT_WIDTH, 0};
        DownRight += sf::Vector2f{CRAFTING_INGREDIENT_WIDTH, 0};
      }

      start_y += CRAFTING_ICON_HEIGHT;
    }
  }

  craftable_list = crafting.getCraftable(item_quantities, crafting_flags);
  int i;
  for (i = 0; i < craftable_list.size(); i++) {

    sf::Vector2f texStart = craftable_list[i].output.getTexturePosition();
    sf::Vector2f UpLeft = texStart,
                 UpRight(texStart.x + TEXTURE_WIDTH, texStart.y),
                 DownLeft(texStart.x, texStart.y + TEXTURE_HEIGHT),
                 DownRight(texStart.x + TEXTURE_WIDTH,
                           texStart.y + TEXTURE_HEIGHT);

    int index = i * (VERTICES_PER_SQUARE * (crafting.getMaxIngredients() + 1));
    crafting_array[index + 0].color = sf::Color(0xFFFFFFFF);
    crafting_array[index + 1].color = sf::Color(0xFFFFFFFF);
    crafting_array[index + 2].color = sf::Color(0xFFFFFFFF);
    crafting_array[index + 3].color = sf::Color(0xFFFFFFFF);
    crafting_array[index + 4].color = sf::Color(0xFFFFFFFF);
    crafting_array[index + 5].color = sf::Color(0xFFFFFFFF);

    crafting_array[index + 0].texCoords = UpLeft;
    crafting_array[index + 1].texCoords = UpRight;
    crafting_array[index + 2].texCoords = DownRight;
    crafting_array[index + 3].texCoords = DownRight;
    crafting_array[index + 4].texCoords = DownLeft;
    crafting_array[index + 5].texCoords = UpLeft;

    for (int ingredient = 0; ingredient < crafting.getMaxIngredients();
         ingredient++) {
      index += 6;
      if (ingredient >= craftable_list[i].ingredients.size()) {
        crafting_array[index + 0].color = sf::Color(TRANSPARENT);
        crafting_array[index + 1].color = sf::Color(TRANSPARENT);
        crafting_array[index + 2].color = sf::Color(TRANSPARENT);
        crafting_array[index + 3].color = sf::Color(TRANSPARENT);
        crafting_array[index + 4].color = sf::Color(TRANSPARENT);
        crafting_array[index + 5].color = sf::Color(TRANSPARENT);

      } else {

        texStart = ItemList[craftable_list[i].ingredients[ingredient].first]
                       .getTexturePosition();
        UpLeft = texStart;
        UpRight = texStart + sf::Vector2f{TEXTURE_WIDTH, 0};
        DownLeft = texStart + sf::Vector2f{0, TEXTURE_HEIGHT};
        DownRight = texStart + sf::Vector2f{TEXTURE_WIDTH, TEXTURE_HEIGHT};
        crafting_array[index + 0].color = sf::Color(SLOT_COLOR);
        crafting_array[index + 1].color = sf::Color(SLOT_COLOR);
        crafting_array[index + 2].color = sf::Color(SLOT_COLOR);
        crafting_array[index + 3].color = sf::Color(SLOT_COLOR);
        crafting_array[index + 4].color = sf::Color(SLOT_COLOR);
        crafting_array[index + 5].color = sf::Color(SLOT_COLOR);
        crafting_array[index + 0].texCoords = UpLeft;
        crafting_array[index + 1].texCoords = UpRight;
        crafting_array[index + 2].texCoords = DownRight;
        crafting_array[index + 3].texCoords = DownRight;
        crafting_array[index + 4].texCoords = DownLeft;
        crafting_array[index + 5].texCoords = UpLeft;
      }
    }
  }

  for (; i < CRAFTING_RECIPES_SHOWN; i++) {

    int index = i * (VERTICES_PER_SQUARE * (crafting.getMaxIngredients() + 1));
    crafting_array[index + 0].color = sf::Color(TRANSPARENT);
    crafting_array[index + 1].color = sf::Color(TRANSPARENT);
    crafting_array[index + 2].color = sf::Color(TRANSPARENT);
    crafting_array[index + 3].color = sf::Color(TRANSPARENT);
    crafting_array[index + 4].color = sf::Color(TRANSPARENT);
    crafting_array[index + 5].color = sf::Color(TRANSPARENT);

    for (int ingredient = 0; ingredient < crafting.getMaxIngredients();
         ingredient++) {
      index += 6;

      crafting_array[index + 0].color = sf::Color(TRANSPARENT);
      crafting_array[index + 1].color = sf::Color(TRANSPARENT);
      crafting_array[index + 2].color = sf::Color(TRANSPARENT);
      crafting_array[index + 3].color = sf::Color(TRANSPARENT);
      crafting_array[index + 4].color = sf::Color(TRANSPARENT);
      crafting_array[index + 5].color = sf::Color(TRANSPARENT);
    }
  }
}

int Inventory::getCraftingFromPosition(sf::Vector2i pos) {
  if (pos.x < CRAFTING_START.x ||
      pos.x > CRAFTING_START.x + CRAFTING_ICON_WIDTH)
    return -1;
  if (pos.y < CRAFTING_START.y)

    return -1;

  for (int i = 0; i < CRAFTING_RECIPES_SHOWN; i++) {
    int index = i * (VERTICES_PER_SQUARE * (crafting.getMaxIngredients() + 1));
    if (pos.y >= crafting_array[index].position.y &&
        pos.y <= (crafting_array[index].position.y + CRAFTING_ICON_HEIGHT)) {
      return i;
    }
  }
  return -1;
}

////////////////////////////////////////////////////////////////////////////////
// Public Methods
////////////////////////////////////////////////////////////////////////////////

// Constructor
Inventory::Inventory() {

  sf::Vector2f vec(10.f, 50.f);

  // Init buttons needed
  Controls::addButton(sf::Keyboard::Key::E);
  Controls::addButton(sf::Keyboard::Key::F);
  Controls::addButton(sf::Keyboard::Key::Num1);
  Controls::addButton(sf::Keyboard::Key::Num2);
  Controls::addButton(sf::Keyboard::Key::Num3);
  Controls::addButton(sf::Keyboard::Key::Num4);
  Controls::addButton(sf::Keyboard::Key::Num5);

  if (DEBUG) {
    Controls::addButton(sf::Keyboard::Key::I); // Debug print inventory
    Controls::addButton(sf::Keyboard::Key::P); // Debug add items
    Controls::addButton(sf::Keyboard::Key::L); // Debug set fire flag
  }

  // Don't ask why I have this as a param, and not a define like everything else
  start = vec;

  bool t = item_texture.loadFromFile("res/items.png");
  if (!t) {
    std::cerr << "Items image did not load" << std::endl;
  }

  for (int i = 0; i < SLOTS; i++) {
    slot_filled[i] = false;
    inventory.at(i) = Item();
  }
  for (int i = 0; i < 8; i++) {
    flag_counts[i] = 0;
  }
  moving = false;
  open = false;
  crafting_flags = 0;
  crafting_position = 0;
  hotbar_position = 0;
  floating_item = Item();
  equipment = Item();

  // NOTE:
  // Explanation of vertexCount:
  // SLOTS  -- number of inventory slots
  // * 2    -- One for the base layer, one for the items layer
  // + 1    -- Background rectangle
  // + 1    -- Equipment background rectangle
  // + 2    -- Equipment Slot base and item layers
  array = sf::VertexArray(sf::PrimitiveType::Triangles,
                          ((SLOTS * 2) + 6) * VERTICES_PER_SQUARE);
  floating_icon =
      sf::VertexArray(sf::PrimitiveType::Triangles, VERTICES_PER_SQUARE);

  sf::Vector2f UpLeft(start.x, start.y), UpRight(start.x + WIDTH, start.y),
      DownLeft(start.x, start.y + TOTAL_HEIGHT),
      DownRight(start.x + WIDTH, start.y + TOTAL_HEIGHT);

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
  float border_height = slot_height * .05;

  // Base Layer
  for (int slot = 0; slot < SLOTS; slot++) {
    if (slot % COLUMNS == 0 && slot != 0) {
      x = start.x;
      y = y + slot_height;
      if (slot == COLUMNS) {
        y += HOTBAR_BUFFER;
      }
    }

    int index = slot * 6 + 6;
    sf::Vector2f UpLeft(x + border_width, y + border_height),
        UpRight(x + slot_width - border_width, y + border_height),
        DownLeft(x + border_width, y + slot_height - border_height),
        DownRight(x + slot_width - border_width,
                  y + slot_width - border_height);

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

  // Textures
  for (int i = 0; i < 5; i++) {
    int index = i * 6 + 6;
    sf::Vector2f UpLeft((32 * i), 32), UpRight((32 * (i + 1)), 32),
        DownLeft((32 * i), 64), DownRight((32 * (i + 1)), 64);

    array[index + 0].texCoords = UpLeft;
    array[index + 1].texCoords = UpRight;
    array[index + 2].texCoords = DownRight;
    array[index + 3].texCoords = DownRight;
    array[index + 4].texCoords = DownLeft;
    array[index + 5].texCoords = UpLeft;
  }

  x = start.x;
  y = start.y;
  // Item Layer
  for (int slot = 0; slot < SLOTS; slot++) {
    if (slot % COLUMNS == 0 && slot != 0) {
      x = start.x;
      y = y + slot_height;
      if (slot == COLUMNS) {
        y += HOTBAR_BUFFER;
      }
    }

    sf::Vector2f UpLeft(x + border_width, y + border_height),
        UpRight(x + slot_width - border_width, y + border_height),
        DownLeft(x + border_width, y + slot_height - border_height),
        DownRight(x + slot_width - border_width,
                  y + slot_width - border_height);

    int index_start = (SLOTS + 1) * 6;
    int index = slot * 6 + index_start;
    array[index + 0].position = UpLeft;
    array[index + 1].position = UpRight;
    array[index + 2].position = DownRight;
    array[index + 3].position = DownRight;
    array[index + 4].position = DownLeft;
    array[index + 5].position = UpLeft;
    array[index + 0].color = sf::Color(TRANSPARENT);
    array[index + 1].color = sf::Color(TRANSPARENT);
    array[index + 2].color = sf::Color(TRANSPARENT);
    array[index + 3].color = sf::Color(TRANSPARENT);
    array[index + 4].color = sf::Color(TRANSPARENT);
    array[index + 5].color = sf::Color(TRANSPARENT);

    //  }
    x += slot_width;
  }

  // Equipment slot
  int index = EQUIP_VERTEX_INDEX;
  x = start.x + WIDTH;
  y = start.y;

  UpLeft = sf::Vector2f{x, y};
  UpRight = sf::Vector2f{x + SLOT_SIZE, y};
  DownLeft = sf::Vector2f{x, y + SLOT_SIZE};
  DownRight = sf::Vector2f{x + SLOT_SIZE, y + SLOT_SIZE};

  array[index + 0].position = UpLeft;
  array[index + 1].position = UpRight;
  array[index + 2].position = DownRight;
  array[index + 3].position = DownRight;
  array[index + 4].position = DownLeft;
  array[index + 5].position = UpLeft;
  array[index + 0].color = sf::Color(BACKGROUND_COLOR);
  array[index + 1].color = sf::Color(BACKGROUND_COLOR);
  array[index + 2].color = sf::Color(BACKGROUND_COLOR);
  array[index + 3].color = sf::Color(BACKGROUND_COLOR);
  array[index + 4].color = sf::Color(BACKGROUND_COLOR);
  array[index + 5].color = sf::Color(BACKGROUND_COLOR);

  index += 6;

  UpLeft = sf::Vector2f(160, 32);
  UpRight = sf::Vector2f(192, 32);
  DownLeft = sf::Vector2f(160, 64);
  DownRight = sf::Vector2f(192, 64);

  array[index + 0].texCoords = UpLeft;
  array[index + 1].texCoords = UpRight;
  array[index + 2].texCoords = DownRight;
  array[index + 3].texCoords = DownRight;
  array[index + 4].texCoords = DownLeft;
  array[index + 5].texCoords = UpLeft;
  UpLeft = sf::Vector2f{x + border_width, y + border_height};
  UpRight = sf::Vector2f{x + SLOT_SIZE - border_width, y + border_height};
  DownLeft = sf::Vector2f{x + border_width, y + SLOT_SIZE - border_height};
  DownRight = sf::Vector2f{x + SLOT_SIZE + -border_width,
                           y + SLOT_SIZE - border_height};
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

  index += 6;
  array[index + 0].position = UpLeft;
  array[index + 1].position = UpRight;
  array[index + 2].position = DownRight;
  array[index + 3].position = DownRight;
  array[index + 4].position = DownLeft;
  array[index + 5].position = UpLeft;
  array[index + 0].color = sf::Color(TRANSPARENT);
  array[index + 1].color = sf::Color(TRANSPARENT);
  array[index + 2].color = sf::Color(TRANSPARENT);
  array[index + 3].color = sf::Color(TRANSPARENT);
  array[index + 4].color = sf::Color(TRANSPARENT);
  array[index + 5].color = sf::Color(TRANSPARENT);

  // NOTE:
  // Explanation of vertexCount:
  // CRAFTING_RECIPES_SHOWN   -- Number of recipes shown
  // * maxIngredients + 1     -- Number of items per crafting recipe
  //                          -- number of the ingredients + 1 for the result
  max_ingredients = crafting.getMaxIngredients();

  crafting_array = sf::VertexArray(
      sf::PrimitiveType::Triangles,
      (CRAFTING_RECIPES_SHOWN * (crafting.getMaxIngredients() + 1)) *
          VERTICES_PER_SQUARE);

  float start_x = CRAFTING_START.x;
  float start_y = CRAFTING_START.y;

  for (int recipe = 0; recipe < CRAFTING_RECIPES_SHOWN; recipe++) {
    x = start_x;
    y = start_y;
    slot_width = CRAFTING_ICON_WIDTH;
    slot_height = CRAFTING_ICON_HEIGHT;

    border_width = slot_width * .05;
    border_height = slot_height * .05;

    sf::Vector2f UpLeft(x + border_width, y + border_height),
        UpRight(x + slot_width - border_width, y + border_height),
        DownLeft(x + border_width, y + slot_height - border_height),
        DownRight(x + slot_width - border_width,
                  y + slot_width - border_height);

    // Just trust me, this is correct
    int index =
        (recipe * (VERTICES_PER_SQUARE * (crafting.getMaxIngredients() + 1)));

    crafting_array[index + 0].position = UpLeft;
    crafting_array[index + 1].position = UpRight;
    crafting_array[index + 2].position = DownRight;
    crafting_array[index + 3].position = DownRight;
    crafting_array[index + 4].position = DownLeft;
    crafting_array[index + 5].position = UpLeft;

    crafting_array[index + 0].color = sf::Color(TRANSPARENT);
    crafting_array[index + 1].color = sf::Color(TRANSPARENT);
    crafting_array[index + 2].color = sf::Color(TRANSPARENT);
    crafting_array[index + 3].color = sf::Color(TRANSPARENT);
    crafting_array[index + 4].color = sf::Color(TRANSPARENT);
    crafting_array[index + 5].color = sf::Color(TRANSPARENT);

    // Prepare for the smaller icons
    border_width = CRAFTING_INGREDIENT_WIDTH * .05;
    border_height = CRAFTING_INGREDIENT_HEIGHT * .05;

    UpLeft = sf::Vector2f(
        start_x + CRAFTING_ICON_WIDTH,
        start_y + ((CRAFTING_ICON_HEIGHT - CRAFTING_INGREDIENT_HEIGHT) / 2) +
            border_height);
    UpRight =
        UpLeft + sf::Vector2f{CRAFTING_INGREDIENT_WIDTH - border_width, 0};
    DownLeft =
        UpLeft + sf::Vector2f{0, CRAFTING_INGREDIENT_HEIGHT - border_height};
    DownRight =
        DownLeft + sf::Vector2f{CRAFTING_INGREDIENT_WIDTH - border_width, 0};

    for (int ingredient = 0; ingredient < crafting.getMaxIngredients();
         ingredient++) {
      index += 6;

      crafting_array[index + 0].position = UpLeft;
      crafting_array[index + 1].position = UpRight;
      crafting_array[index + 2].position = DownRight;
      crafting_array[index + 3].position = DownRight;
      crafting_array[index + 4].position = DownLeft;
      crafting_array[index + 5].position = UpLeft;

      crafting_array[index + 0].color = sf::Color(TRANSPARENT);
      crafting_array[index + 1].color = sf::Color(TRANSPARENT);
      crafting_array[index + 2].color = sf::Color(TRANSPARENT);
      crafting_array[index + 3].color = sf::Color(TRANSPARENT);
      crafting_array[index + 4].color = sf::Color(TRANSPARENT);
      crafting_array[index + 5].color = sf::Color(TRANSPARENT);

      UpLeft += sf::Vector2f{CRAFTING_INGREDIENT_WIDTH, 0};
      UpRight += sf::Vector2f{CRAFTING_INGREDIENT_WIDTH, 0};
      DownLeft += sf::Vector2f{CRAFTING_INGREDIENT_WIDTH, 0};
      DownRight += sf::Vector2f{CRAFTING_INGREDIENT_WIDTH, 0};
    }

    start_y += CRAFTING_ICON_HEIGHT;
  }
  hotbar = sf::VertexArray(sf::PrimitiveType::Triangles,
                           ((HOTBAR_SLOTS * 2) + 2) * VERTICES_PER_SQUARE);

  UpLeft = sf::Vector2f{start.x, start.y};
  UpRight = sf::Vector2f{start.x + WIDTH, start.y};
  DownLeft = sf::Vector2f{start.x, start.y + SLOT_SIZE};
  DownRight = sf::Vector2f{start.x + WIDTH, start.y + SLOT_SIZE};

  hotbar[0].position = UpLeft;
  hotbar[1].position = UpRight;
  hotbar[2].position = DownRight;
  hotbar[3].position = DownRight;
  hotbar[4].position = DownLeft;
  hotbar[5].position = UpLeft;

  slot_width = WIDTH / COLUMNS;
  slot_height = HEIGHT / ROWS;

  border_width = slot_width * .05;
  border_height = slot_height * .05;
  x = start.x;
  y = start.y;

  UpRight = sf::Vector2f{start.x + SLOT_SIZE, start.y};
  DownLeft = sf::Vector2f{start.x, start.y + SLOT_SIZE};
  DownRight = sf::Vector2f{start.x + SLOT_SIZE, start.y + SLOT_SIZE};

  hotbar[6 + 0].position = UpLeft;
  hotbar[6 + 1].position = UpRight;
  hotbar[6 + 2].position = DownRight;
  hotbar[6 + 3].position = DownRight;
  hotbar[6 + 4].position = DownLeft;
  hotbar[6 + 5].position = UpLeft;
  hotbar[6 + 0].color = sf::Color(HIGHLIGHT_COLOR);
  hotbar[6 + 1].color = sf::Color(HIGHLIGHT_COLOR);
  hotbar[6 + 2].color = sf::Color(HIGHLIGHT_COLOR);
  hotbar[6 + 3].color = sf::Color(HIGHLIGHT_COLOR);
  hotbar[6 + 4].color = sf::Color(HIGHLIGHT_COLOR);
  hotbar[6 + 5].color = sf::Color(HIGHLIGHT_COLOR);

  // BAckground
  for (int slot = 0; slot < HOTBAR_SLOTS; slot++) {
    int index = (slot + 2) * VERTICES_PER_SQUARE;

    sf::Vector2f UpLeft(x + border_width, y + border_height),
        UpRight(x + slot_width - border_width, y + border_height),
        DownLeft(x + border_width, y + slot_height - border_height),
        DownRight(x + slot_width - border_width,
                  y + slot_width - border_height);

    hotbar[index + 0].position = UpLeft;
    hotbar[index + 1].position = UpRight;
    hotbar[index + 2].position = DownRight;
    hotbar[index + 3].position = DownRight;
    hotbar[index + 4].position = DownLeft;
    hotbar[index + 5].position = UpLeft;
    hotbar[index + 0].color = sf::Color(SLOT_COLOR);
    hotbar[index + 1].color = sf::Color(SLOT_COLOR);
    hotbar[index + 2].color = sf::Color(SLOT_COLOR);
    hotbar[index + 3].color = sf::Color(SLOT_COLOR);
    hotbar[index + 4].color = sf::Color(SLOT_COLOR);
    hotbar[index + 5].color = sf::Color(SLOT_COLOR);

    UpLeft = sf::Vector2f((32 * slot), 32);
    UpRight = sf::Vector2f((32 * (slot + 1)), 32);
    DownLeft = sf::Vector2f((32 * slot), 64);
    DownRight = sf::Vector2f((32 * (slot + 1)), 64);

    hotbar[index + 0].texCoords = UpLeft;
    hotbar[index + 1].texCoords = UpRight;
    hotbar[index + 2].texCoords = DownRight;
    hotbar[index + 3].texCoords = DownRight;
    hotbar[index + 4].texCoords = DownLeft;
    hotbar[index + 5].texCoords = UpLeft;
    x += SLOT_SIZE;
  }

  x = start.x;
  y = start.y;
  // Item
  for (int slot = 0; slot < HOTBAR_SLOTS; slot++) {
    int index =
        (HOTBAR_SLOTS + 2) * VERTICES_PER_SQUARE + slot * VERTICES_PER_SQUARE;

    sf::Vector2f UpLeft(x + border_width, y + border_height),
        UpRight(x + slot_width - border_width, y + border_height),
        DownLeft(x + border_width, y + slot_height - border_height),
        DownRight(x + slot_width - border_width,
                  y + slot_width - border_height);

    hotbar[index + 0].position = UpLeft;
    hotbar[index + 1].position = UpRight;
    hotbar[index + 2].position = DownRight;
    hotbar[index + 3].position = DownRight;
    hotbar[index + 4].position = DownLeft;
    hotbar[index + 5].position = UpLeft;
    hotbar[index + 0].color = sf::Color(TRANSPARENT);
    hotbar[index + 1].color = sf::Color(TRANSPARENT);
    hotbar[index + 2].color = sf::Color(TRANSPARENT);
    hotbar[index + 3].color = sf::Color(TRANSPARENT);
    hotbar[index + 4].color = sf::Color(TRANSPARENT);
    hotbar[index + 5].color = sf::Color(TRANSPARENT);
    x += SLOT_SIZE;
  }
}

// Destructor
Inventory::~Inventory() {
  // idk, I'm not using pointers ..
}

// returns the bool if the inventory is open
bool Inventory::isOpen() { return open; }

// Toggles the displaying of the inventory screen
void Inventory::toggleOpen() {
  open ^= 1;
  crafting_position = 0;
}

// Gets the vertex array
sf::VertexArray Inventory::getArray() { return array; }

// Insert item at position
void Inventory::insertItem(int pos, Item item) {
  if (pos < 0 || pos > SLOTS) {
    return;
  }
  item_quantities[item.getName()] += item.getQuantity();
  slot_filled[pos] = true;
  inventory[pos] = item;
  updateTextures(pos);
  updateCraftableList();
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
  // item_quantities[item.getName()] -= item.getQuantity();
  updateCraftableList();
}

// Generic remove item from inventory
// Is unsafe, will not check if there is enough items already in inventory
void Inventory::removeItem(std::string item, int qty) {

  for (int i = 0; i < SLOTS; i++) {
    if (!slot_filled[i]) {
      continue;
    }
    if (inventory[i].getName() != item) {
      continue;
    }
    if (inventory[i].getQuantity() > qty) {
      inventory[i].addQuantity(-qty);
      item_quantities[item] -= qty;
      break;
    } else if (inventory[i].getQuantity() == qty) {
      {
        removeItem(i);
        break;
      }
    } else {
      qty -= inventory[i].getQuantity();
      removeItem(i);
    }
  }
  updateCraftableList();
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
          item_quantities[item.getName()] += item.getQuantity();
          i->addQuantity(item.getQuantity());
          item.setQuantity(0);
          updateCraftableList();
          return;
        }
        i->addQuantity(empty);
        item.addQuantity(-empty);
        item_quantities[item.getName()] += empty;
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

int Inventory::getHotbarPosition() { return hotbar_position; }

Item Inventory::getHotbarItem() { return inventory[hotbar_position]; }

void Inventory::decHotbarItem() {
  removeItem(inventory[hotbar_position].getName(), 1);
}

void Inventory::moveHotbarRight() {
  hotbar_position++;
  if (hotbar_position >= COLUMNS) {
    hotbar_position = COLUMNS - 1;
  }
  sf::Vector2f UpLeft =
      sf::Vector2f{start.x + (SLOT_SIZE * hotbar_position), start.y};
  sf::Vector2f UpRight = UpLeft + sf::Vector2f{0, SLOT_SIZE};
  sf::Vector2f DownLeft = UpLeft + sf::Vector2f{SLOT_SIZE, 0};
  sf::Vector2f DownRight = UpLeft + sf::Vector2f{SLOT_SIZE, SLOT_SIZE};

  hotbar[6 + 0].position = UpLeft;
  hotbar[6 + 1].position = UpRight;
  hotbar[6 + 2].position = DownRight;
  hotbar[6 + 3].position = DownRight;
  hotbar[6 + 4].position = DownLeft;
  hotbar[6 + 5].position = UpLeft;
}

void Inventory::moveHotbarLeft() {
  hotbar_position--;
  if (hotbar_position <= 0) {
    hotbar_position = 0;
  }
  sf::Vector2f UpLeft =
      sf::Vector2f{start.x + (SLOT_SIZE * hotbar_position), start.y};
  sf::Vector2f UpRight = UpLeft + sf::Vector2f{0, SLOT_SIZE};
  sf::Vector2f DownLeft = UpLeft + sf::Vector2f{SLOT_SIZE, 0};
  sf::Vector2f DownRight = UpLeft + sf::Vector2f{SLOT_SIZE, SLOT_SIZE};

  hotbar[6 + 0].position = UpLeft;
  hotbar[6 + 1].position = UpRight;
  hotbar[6 + 2].position = DownRight;
  hotbar[6 + 3].position = DownRight;
  hotbar[6 + 4].position = DownLeft;
  hotbar[6 + 5].position = UpLeft;
}
void Inventory::setHotbarPosition(int pos) {
  if (pos >= COLUMNS)
    hotbar_position = COLUMNS - 1;
  else if (pos <= 0)
    hotbar_position = 0;
  else
    hotbar_position = pos;

  sf::Vector2f UpLeft =
      sf::Vector2f{start.x + (SLOT_SIZE * hotbar_position), start.y};
  sf::Vector2f UpRight = UpLeft + sf::Vector2f{0, SLOT_SIZE};
  sf::Vector2f DownLeft = UpLeft + sf::Vector2f{SLOT_SIZE, 0};
  sf::Vector2f DownRight = UpLeft + sf::Vector2f{SLOT_SIZE, SLOT_SIZE};

  hotbar[6 + 0].position = UpLeft;
  hotbar[6 + 1].position = UpRight;
  hotbar[6 + 2].position = DownRight;
  hotbar[6 + 3].position = DownRight;
  hotbar[6 + 4].position = DownLeft;
  hotbar[6 + 5].position = UpLeft;
}

// Sets the current crafting flags - shows what objects we're close to
void Inventory::setCraftingFlags(CraftingFlags flags) {
  crafting_flags = flags;
  updateCraftableList();
}

void Inventory::craftingFlagSetFire(bool b) {
  if (b) {
    crafting_flags |= FIRE;
  } else {
    crafting_flags &= ~FIRE;
  }
}

// Debug printing to console the craftable items given current item quantities
void Inventory::printCrafting() {
  updateCraftableList();
  auto vec = crafting.getCraftable(item_quantities, crafting_flags);

  std::cout << "Craftable with flags " << std::bitset<8>(crafting_flags)
            << std::dec;
  for (auto it = vec.begin(); it != vec.end(); it++) {
    std::cout << "Can craft " << it->output.getName() << std::endl;
  }
  std::cout << std::flush;
}

// Debug printing to console the contents item_quantities
void Inventory::printInventory() {
  std::cout << "\nitem_quantities: \n";
  for (auto it = item_quantities.begin(); it != item_quantities.end(); it++) {
    std::cout << it->first << " x" << it->second << '\n';
  }
  std::cout << "Inventory:\n";
  for (auto it = inventory.begin(); it != inventory.end(); it++)
    // std::cout << it->getName() << " x" << it->getQuantity() << '\n';
    it->debugPrint();
}

////////////////////////////////////////////////////////////////////////////////
// Draw functions
////////////////////////////////////////////////////////////////////////////////

// Draws the inventory
void Inventory::draw(sf::RenderWindow &window) {

  sf::Vector2i mousePos = Controls::mousePosition();

  if (Controls::tapped(sf::Keyboard::Key::E)) {
    toggleOpen();
  }

  if (Controls::tapped(sf::Keyboard::Key::L)) {
    craftingFlagSetFire(true);
  }

  if (Controls::tapped(sf::Keyboard::Key::P)) {
    Item item = ItemList["Campfire"];
    item.setQuantity(1);
    // addItem(item);
    item = ItemList["Pickaxe"];
    item.setQuantity(1);
    addItem(item);
    item = ItemList["Axe"];
    item.setQuantity(1);
    addItem(item);
    item = ItemList["Stick"];
    item.setQuantity(50);
    addItem(item);
    item = ItemList["Stone"];
    item.setQuantity(50);
    addItem(item);
    item = ItemList["Carrot"];
    item.setQuantity(50);
    addItem(item);
    item = ItemList["Raw Meat"];
    item.setQuantity(50);
    addItem(item);
    item = ItemList["Bowl"];
    item.setQuantity(50);
    addItem(item);
    item = ItemList["Big Metal Pipe"];
    item.setQuantity(50);
    addItem(item);
    item = ItemList["Fuel"];
    item.setQuantity(50);
    addItem(item);
    item = ItemList["Ignition"];
    item.setQuantity(50);
    addItem(item);
    item = ItemList["Hide"];
    item.setQuantity(50);
    addItem(item);
    item = ItemList["Cooked Meat"];
    item.setQuantity(50);
    addItem(item);
  }

  if (Controls::tapped(sf::Keyboard::Key::I)) {
    printInventory();
  }

  // Hotbar
  if (!open) {

    // Use Function
    if (Controls::tapped(sf::Keyboard::Key::F)) {
      if (inventory[hotbar_position].getActionFunction()) {
        inventory[hotbar_position].useActionFunction();
        decHotbarItem();
      }
    } else if (Controls::tapped(sf::Keyboard::Key::Num1)) {
      setHotbarPosition(0);
    } else if (Controls::tapped(sf::Keyboard::Key::Num2)) {
      setHotbarPosition(1);
    } else if (Controls::tapped(sf::Keyboard::Key::Num3)) {
      setHotbarPosition(2);
    } else if (Controls::tapped(sf::Keyboard::Key::Num4)) {
      setHotbarPosition(3);
    } else if (Controls::tapped(sf::Keyboard::Key::Num5)) {
      setHotbarPosition(4);
    }

    window.draw(hotbar, &item_texture);

    for (int slot = 0; slot < HOTBAR_SLOTS; slot++) {
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

      window.draw(text, &item_texture);
    }
    return;
  }
  drawCrafting(window);
  if (Controls::tapped(sf::Mouse::Button::Right)) {
    if (!moving) {
      int index = getVertexFromPosition(mousePos);
      if (index >= 0 && slot_filled[index]) {
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
      if (index == -2) {
        floating_item = equipment;
        equipment = Item();
        updateTextures(FLOATING_INDEX);
        updateTextures(EQUIPMENT_INDEX);
        StatsContainer::stats.defense -= floating_item.getDefenseGained();
        StatsContainer::stats.cold_resist -= floating_item.getColdResist();
        if (floating_item.getName() != "")
          moving = true;
      } else if (slot_filled[index]) {
        moving = true;
        if (index >= 0) {
          floating_item = inventory[index];
          removeItem(index);
          updateTextures(index);
          updateTextures(FLOATING_INDEX);
        }
      }
      index = getCraftingFromPosition(mousePos);
      if (index != -1 && index + crafting_position < craftable_list.size()) {
        floating_item = craftable_list[index + crafting_position].output;
        moving = true;
        updateTextures(FLOATING_INDEX);
        int end = craftable_list[index + crafting_position].ingredients.size();
        auto list = craftable_list[index + crafting_position].ingredients;
        for (int i = 0; i < end; i++) {

          auto j = list[i];
          // printInventory();
          removeItem(j.first, j.second);
        }
      }
    } else { // Currently moving an item
      if (index == EQUIPMENT_INDEX) {
        uint8_t type = floating_item.getType();
        if ((type & EQUIPABLE) == EQUIPABLE) {

          Item temp = equipment;
          equipment = floating_item;
          floating_item = temp;

          if (floating_item.getName() == "")
            moving = false;

          updateTextures(FLOATING_INDEX);
          updateTextures(EQUIPMENT_INDEX);

          StatsContainer::stats.defense -= floating_item.getDefenseGained();
          StatsContainer::stats.cold_resist -= floating_item.getColdResist();
          StatsContainer::stats.defense += equipment.getDefenseGained();
          StatsContainer::stats.cold_resist += equipment.getColdResist();
        }
      } else {
        if (index == -1) {
          index = getCraftingFromPosition(mousePos);
          if (index != -1) {
            if (floating_item.getName() ==
                craftable_list[index + crafting_position].output.getName()) {
              int empty =
                  floating_item.getMaxStack() - floating_item.getQuantity();
              if (empty > craftable_list[index + crafting_position]
                              .output.getQuantity()) {
                floating_item.addQuantity(
                    craftable_list[index + crafting_position]
                        .output.getQuantity());
                updateTextures(FLOATING_INDEX);
                int end = craftable_list[index + crafting_position]
                              .ingredients.size();
                auto list =
                    craftable_list[index + crafting_position].ingredients;
                for (int i = 0; i < end; i++) {

                  auto j = list[i];
                  // printInventory();
                  removeItem(j.first, j.second);
                }
              }
            }
          }
        } else if (inventory[index].getName() == floating_item.getName()) {
          // items are the same, add to stack
          int empty =
              inventory[index].getMaxStack() - inventory[index].getQuantity();
          if (empty > 0) {
            if (empty > floating_item.getQuantity()) {
              inventory[index].addQuantity(floating_item.getQuantity());
              item_quantities[inventory[index].getName()] +=
                  floating_item.getQuantity();
              floating_item = Item();
              moving = false;
            } else {
              inventory[index].addQuantity(empty);
              item_quantities[inventory[index].getName()] += empty;

              floating_item.addQuantity(-empty); // lol I guess this works
            }
          }
        }

        else {
          // Swap items

          int index = getVertexFromPosition(mousePos);

          if (index >= 0) {
            bool temp_filled = slot_filled[index];
            Item temp_item = inventory[index];
            moving = slot_filled[index];

            slot_filled[index] = true;
            inventory[index] = floating_item;
            item_quantities[floating_item.getName()] +=
                floating_item.getQuantity();

            if (temp_filled) {
              floating_item = temp_item;
              item_quantities[temp_item.getName()] -= temp_item.getQuantity();
            } else {
              floating_item = Item();
            }

            updateTextures(FLOATING_INDEX);
            updateTextures(index);
          }
        }
      }
    }
    updateCraftableList();
  }

  window.draw(array, &item_texture);

  for (int slot = 0; slot < SLOTS; slot++) {
    if (inventory[slot].getQuantity() <= 1)
      continue;
    sf::Vector2f position = array[VertexArrayIndexFromSlot(slot) + 2].position;

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

    window.draw(text, &item_texture);
  }

  // Floating item, Must be last!
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
    window.draw(floating_icon, &item_texture);

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
    Item item = Item();
    if (index == EQUIPMENT_INDEX) {
      item = equipment;
    }
    if (index >= 0 && slot_filled[index]) {
      item = inventory[index];
    }
    if (item.getName() != "") {
      sf::Text text(font, item.getTooltip());
      text.setPosition(sf::Vector2f{static_cast<float>(mousePos.x),
                                    static_cast<float>(mousePos.y)});
      text.setFillColor(sf::Color(TEXT_FILL));
      text.setOutlineColor(sf::Color(TEXT_OUTLINE_COLOR));
      text.setOutlineThickness(TEXT_OUTLINE_SIZE);
      text.setScale(sf::Vector2f{0.9, 0.9});
      text.setLineSpacing(0.9f);

      sf::FloatRect fr = text.getGlobalBounds();

      text.move(sf::Vector2f{TOOLTIP_OFFSET_LEFT, 0});
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

void Inventory::drawCrafting(sf::RenderWindow &window) {
  sf::Text text(font, "Crafting: ");
  text.setFillColor(sf::Color::Black);
  text.setPosition(CRAFTING_START + sf::Vector2f{0.f, -30.f});
  text.setStyle(sf::Text::Style::Bold);
  window.draw(text);

  window.draw(crafting_array, &item_texture);
  float main_scale = (CRAFTING_ICON_WIDTH / SLOT_SIZE);
  float ingr_scale = (CRAFTING_INGREDIENT_WIDTH / SLOT_SIZE);
  for (int i = 0; i < CRAFTING_RECIPES_SHOWN && i < craftable_list.size();
       i++) {
    int index = CRAFTING_INDEX(i) + 2;
    if (craftable_list[i].output.getQuantity() > 1) {
      sf::Vector2f position = crafting_array[index].position;
      sf::Text text(font,
                    std::to_string(craftable_list[i].output.getQuantity()));

      // text.scale(sf::Vector2f{main_scale, main_scale});
      unsigned int size = 30 * main_scale;
      text.setCharacterSize(size);

      position.y -= size;
      // Compensate for either 1 or 2 digits
      if (craftable_list[i].output.getQuantity() / 10 > 0) {
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
    for (int j = 0; j < crafting.getMaxIngredients() &&
                    j < craftable_list[i].ingredients.size();
         j++) {

      index += 6;
      int qty = craftable_list[i].ingredients[j].second;

      if (qty > 1) {
        sf::Vector2f position = crafting_array[index].position;
        sf::Text text(font, std::to_string(qty));

        // text.scale(sf::Vector2f{main_scale, main_scale});
        unsigned int size = 30 * ingr_scale;
        text.setCharacterSize(size);

        position.y -= size;
        // Compensate for either 1 or 2 digits
        if (qty / 10 > 0) {
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
    }
  }
}
