#pragma once
// The whole list of possible default items

#include "Item.hpp"
#include "SFML/System/Vector2.hpp"
#include "Stats.hpp"
#include <map>
#include <sstream>

#define DEFAULT_MAX_STACK_SIZE 10

// A macro for building off of the item list
#define ITEM(name, qty)                                                        \
  Item(name, ItemList[name].getTexturePosition(), qty,                         \
       ItemList[name].getMaxStack(), ItemList[name].getType(),                 \
       ItemList[name].getHpGained(), ItemList[name].getHungerGained(),         \
       ItemList[name].getWarmthGained(), ItemList[name].getDefenseGained(),    \
       ItemList[name].getColdResist(), ItemList[name].getActionFunction(),     \
       ItemList[name].getTooltipFunction())

#define TEXTURE_SIZE 32.f
#define TEXT(x, y)                                                             \
  sf::Vector2f { x *TEXTURE_SIZE, y *TEXTURE_SIZE }

// Shorthands for filling in 0s for the stats
#define NOT_CONSUMABLE 0, 0, 0
#define NOT_EQUIPABLE 0, 0

// Function prototypes for any custom functions needed
std::string soup_tooltip(Item *);
void consumable_action(Item *item);

std::map<std::string, Item> ItemList{

    ////////////////////////////////////////////////////////////////////////////////
    ///    Materials
    ////////////////////////////////////////////////////////////////////////////////

    {"Stick", Item("Stick", TEXT(1, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
                   NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},
    {"Stone", Item("Stone", TEXT(2, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
                   NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},
    {"Rope", Item("Rope", TEXT(3, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
                  NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},
    // Maybe move raw meat to consumables?
    {"Raw Meat", Item("Raw Meat", TEXT(0, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
                      NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},
    {"Grass", Item("Grass", TEXT(0, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
                   NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},
    {"Bowl", Item("Bowl", TEXT(0, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
                  NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},
    {"Hide", Item("Hide", TEXT(0, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
                  NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},
    {"Big Metal Pipe",
     Item("Big Metal Pipe", TEXT(0, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
          NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},
    {"Fuel", Item("Fuel", TEXT(0, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
                  NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},
    {"Ignition", Item("Ignition", TEXT(0, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
                      NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},

    ////////////////////////////////////////////////////////////////////////////////
    ///    Tools
    ////////////////////////////////////////////////////////////////////////////////
    {"Axe", Item("Axe", TEXT(5, 0), 0, 1, 0, NOT_CONSUMABLE, NOT_EQUIPABLE,
                 NULL, NULL)},
    {"Pickaxe", Item("Pickaxe", TEXT(4, 0), 0, 1, 0, NOT_CONSUMABLE,
                     NOT_EQUIPABLE, NULL, NULL)},
    {"Shovel", Item("Shovel", TEXT(7, 0), 0, 1, 0, NOT_CONSUMABLE,
                    NOT_EQUIPABLE, NULL, NULL)},
    {"Sword", Item("Sword", TEXT(6, 0), 0, 1, 0, NOT_CONSUMABLE, NOT_EQUIPABLE,
                   NULL, NULL)},
    {"Campfire", Item("Campfire", TEXT(3, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
                      NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},
    {"Flame Thrower",
     Item("Flame Thrower", TEXT(3, 0), 0, DEFAULT_MAX_STACK_SIZE, 0,
          NOT_CONSUMABLE, NOT_EQUIPABLE, NULL, NULL)},
    ////////////////////////////////////////////////////////////////////////////////
    ///    Equipment
    ////////////////////////////////////////////////////////////////////////////////
    {"Clothes", Item("Clothes", TEXT(3, 0), 0, 1, EQUIPABLE, NOT_CONSUMABLE, 10,
                     10, NULL, NULL)},

    ////////////////////////////////////////////////////////////////////////////////
    ///    Consumables
    ////////////////////////////////////////////////////////////////////////////////
    {"Berries",
     Item("Berries", TEXT(8, 0), 0, DEFAULT_MAX_STACK_SIZE, CONSUMABLE, 1, 2, 0,
          NOT_EQUIPABLE, consumable_action, NULL)},
    {"Carrot", Item("Carrot", TEXT(0, 0), 0, DEFAULT_MAX_STACK_SIZE, CONSUMABLE,
                    1, 2, 0, NOT_EQUIPABLE, NULL, NULL)},
    {"Cooked Meat", Item("Cooked Meat", TEXT(0, 0), 0, DEFAULT_MAX_STACK_SIZE,
                         CONSUMABLE, 4, 6, 2, NOT_EQUIPABLE, NULL, NULL)},
    {"Snowman Soup",
     Item("Snowman Soup", TEXT(0, 0), 0, DEFAULT_MAX_STACK_SIZE, CONSUMABLE, 4,
          6, 4, NOT_EQUIPABLE, NULL, soup_tooltip)},

    ////////////////////////////////////////////////////////////////////////////////
    ///    Misc/Testing
    ////////////////////////////////////////////////////////////////////////////////
    {"All The Things",
     Item("All The Things", TEXT(0, 0), 0, 99,
          CONSUMABLE | TOOL | EQUIPABLE | BLOCK, 1, 1, 1, 1, 1)}

};

////////////////////////////////////////////////////////////////////////////////
///     Custom Action Functions
////////////////////////////////////////////////////////////////////////////////
void consumable_action(Item *item) {

  PlayerStats.health += item->getHpGained();
  PlayerStats.hunger += item->getHungerGained();
  PlayerStats.warmth += item->getWarmthGained();

  if (PlayerStats.health > PlayerStats.max_health) {
    PlayerStats.health = PlayerStats.max_health;
  }

  if (PlayerStats.hunger > PlayerStats.max_hunger) {
    PlayerStats.hunger = PlayerStats.max_hunger;
  }

  if (PlayerStats.warmth > PlayerStats.max_warmth) {
    PlayerStats.warmth = PlayerStats.max_warmth;
  }
}

////////////////////////////////////////////////////////////////////////////////
///     Custom Tooltip Functions
////////////////////////////////////////////////////////////////////////////////

std::string soup_tooltip(Item *item) {
  std::stringstream ss;
  ss << item->getName() << "\nWarm bowl of soup\n"
     << "Made from the blood of your enemies.\n"
     << item->getQuantity() << "/" << item->getMaxStack()
     << "\nHP: " << item->getHpGained() << "\nFood: " << item->getHungerGained()
     << "\nWarmth: " << item->getWarmthGained();

  return ss.str();
}
