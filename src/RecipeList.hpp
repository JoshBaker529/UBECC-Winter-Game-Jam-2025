#pragma once

#include "ItemList.hpp" // Includes Item
#include <utility>
#include <vector>

// Typedef for the list of total items in inventory
typedef std::map<std::string, int> ItemQuantities;
typedef std::vector<std::pair<std::string, int>> Ingredients;

// Typedef for the flags
typedef uint8_t CraftingFlags;

#define NO_FLAG 0
#define CRAFTING_BENCH 0b1
#define FIRE 0b10

// Structure for what a Recipe is
struct Recipe {
  Item output;
  Ingredients ingredients;
  CraftingFlags flags;
};

// The full list of recipes
static std::vector<Recipe> recipes{
    Recipe{ITEM("Axe", 1),
           Ingredients{std::pair("Stick", 3), std::pair("Stone", 3)}, 0},
    Recipe{ITEM("Pickaxe", 1),
           Ingredients{std::pair("Stick", 3), std::pair("Stone", 3)}, 0},
    Recipe{ITEM("Shovel", 1),
           Ingredients{std::pair("Stick", 3), std::pair("Stone", 3)}, 0},
    Recipe{ITEM("Sword", 1),
           Ingredients{std::pair("Stick", 3), std::pair("Stone", 3)}, 0},
    Recipe{ITEM("Bowl", 1), Ingredients{std::pair("Stick", 8)}, 0},
    Recipe{ITEM("Campfire", 1),
           Ingredients{std::pair("Stick", 8), std::pair("Stone", 1)}, 0},

    Recipe{ITEM("Clothes", 1), Ingredients{std::pair("Hide", 8)}, 0},
    Recipe{ITEM("Flame Thrower", 1),
           Ingredients{std::pair("Big Metal Pipe", 1), std::pair("Fuel", 1),
                       std::pair("Ignition", 1)},
           0},
    Recipe{ITEM("Cooked Meat", 1), Ingredients{std::pair("Raw Meat", 1)}, FIRE},
    Recipe{ITEM("Snowman Soup", 1),
           Ingredients{std::pair("Bowl", 1), std::pair("Cooked Meat", 1),
                       std::pair("Carrot", 1), std::pair("Grass", 5)},
           FIRE},

    Recipe{ITEM("idfk", 1),
           Ingredients{std::pair("one", 1), std::pair("two", 2),
                       std::pair("three", 3)},
           0}};
