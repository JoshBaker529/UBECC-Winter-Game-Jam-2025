// This file holds everything involved with crafting;
#pragma once

#include "RecipeList.hpp" // Includes ItemList.hpp, Item.hpp
#include <vector>

// TODO:
// Currently need to fully implement the item_quantities in Inventory.
// Once that's done, I can implement getCraftable.
// Not sure if I want to have Crafting handling rendering the list
// or if Inventory should handle that.

class Crafting {
private:
  int maxIngredients;

  int calclulateMaxIngredients();

public:
  std::vector<Recipe> getCraftable(ItemQuantities, CraftingFlags);
  int getMaxIngredients();

  Crafting();
  Crafting(Crafting &&) = default;
  Crafting(const Crafting &) = default;
  Crafting &operator=(Crafting &&) = default;
  Crafting &operator=(const Crafting &) = default;
  ~Crafting();
};

Crafting::Crafting() { maxIngredients = calclulateMaxIngredients(); }

std::vector<Recipe> Crafting::getCraftable(ItemQuantities items,
                                           CraftingFlags flags) {
  std::vector<Recipe> ret;
  for (auto recipe = recipes.begin(); recipe != recipes.end(); recipe++) {
    bool canCraft = true;
    for (auto ingredient = recipe->ingredients.begin();
         ingredient != recipe->ingredients.end(); ingredient++) {
      if ((flags & recipe->flags) != recipe->flags) {
        canCraft = false;
        break;
      }
      if (items[ingredient->first] < ingredient->second) {
        canCraft = false;
        break;
      }
    }
    if (canCraft) {
      ret.push_back(*recipe);
    }
  }
  return ret;
}

int Crafting::getMaxIngredients() { return maxIngredients; }

int Crafting::calclulateMaxIngredients() {
  int max = 0;
  for (auto it = recipes.begin(); it != recipes.end(); it++) {
    if (it->ingredients.size() > max)
      max = it->ingredients.size();
  }

  return max;
}

Crafting::~Crafting() {}
