#pragma once
#include "ItemList.hpp" // Includes Item
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
    Recipe{ITEM("axe", 1),
           Ingredients{std::pair("stick", 3), std::pair("stone", 3)},
           CRAFTING_BENCH},
    Recipe{ITEM("idfk", 1),
           Ingredients{std::pair("one", 1), std::pair("two", 2),
                       std::pair("three", 3)}}};
