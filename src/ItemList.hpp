// The whole list of possible default items

#include "Item.hpp"
#include "SFML/System/Vector2.hpp"
#include <map>

#define DEFAULT_MAX_STACK_SIZE 10

// A macro for building off of the item list
#define ITEM(name, qty)                                                        \
  Item(name, ItemList[name].getTexturePosition(), qty,                         \
       ItemList[name].getMaxStack(), ItemList[name].getType(),                 \
       ItemList[name].getHpGained(), ItemList[name].getHungerGained(),         \
       ItemList[name].getWarmthGained(), ItemList[name].getDefenseGained(),    \
       ItemList[name].getColdResist())

// Shorthands for filling in 0s for the stats
#define NOT_CONSUMABLE 0, 0, 0
#define NOT_EQUIPABLE 0, 0

std::map<std::string, Item> ItemList{

    ////////////////////////////////////////////////////////////////////////////////
    ///    Materials
    ////////////////////////////////////////////////////////////////////////////////

    {"stick", Item("stick", sf::Vector2f{0, 0}, 0, 10, 0, NOT_CONSUMABLE,
                   NOT_EQUIPABLE, NULL, NULL)},
    {"stone", Item("stone", sf::Vector2f{0, 0}, 0, 10, 0, NOT_CONSUMABLE,
                   NOT_EQUIPABLE, NULL, NULL)},

    ////////////////////////////////////////////////////////////////////////////////
    ///    Tools
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    ///    Equipment
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    ///    Consumables
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    ///    Misc/Testing
    ////////////////////////////////////////////////////////////////////////////////
    {"All The Things",
     Item("All The Things", sf::Vector2f{0, 0}, 0, 99,
          CONSUMABLE | TOOL | EQUIPABLE | BLOCK, 1, 1, 1, 1, 1)}

};
