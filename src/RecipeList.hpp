
#include "ItemList.hpp" // Includes Item
#include <vector>

// Typedef for the list of total items in inventory
typedef std::map<std::string, int> ItemQuantities;

// Typedef for the flags
typedef uint8_t CraftingFlags;

#define NO_FLAG 0
#define CRAFTING_BENCH 0b1
#define FIRE 0b10

// Structure for what a Recipe is
struct Recipe {
  Item output;
  ItemQuantities ingredients;
  CraftingFlags flags;
};

// The full list of recipes
static std::vector<Recipe> recipes{
    Recipe{ITEM("axe", 1),
           ItemQuantities{std::pair("stick", 3), std::pair("stone", 3)},
           CRAFTING_BENCH},
};
