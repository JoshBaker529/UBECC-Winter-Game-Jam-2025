
#include "Item.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"
#include <sstream>
Item::Item(std::string name, sf::Vector2f texture, int max_stack,
           TypeFlags type, actionFunction action, tooltipFunction hover)
    : name(name), texture_position(texture), stack_size(0), max_stack_size(0),
      type(type), hp_gained(0), hunger_gained(0), warmth_gained(0), defense(0),
      cold_resist(0), useAction(action), tooltipHover(hover) {
        // INFO:
        // Constructor intentionally left blank, everything is in initialization
        // list.
        // See setConsumableStats and setEquipableStats for the stats that are
        // not listed here
      };

void Item::setConsumableStats(float hp, float hunger, float warmth) {
  hp_gained = hp;
  hunger_gained = hunger;
  warmth_gained = warmth;
}

void Item::setEquipableStats(float armor, float cold) {
  defense = armor;
  cold_resist = cold;
}

void Item::draw(sf::RenderWindow &window) {
  sf::RectangleShape rect(sf::Vector2f{50, 50});
  rect.setPosition(sf::Vector2f{70, 70});

  window.draw(rect);
}

std::string Item::getTooltip() {
  std::stringstream ss;
  if (tooltipHover) {
    ss << tooltipHover();
    return ss.str();
  }

  ss << name << '\n' << stack_size << " / " << max_stack_size;

  if (type & CONSUMABLE)
    ss << "\nHP: " << hp_gained << "\nFood: " << hunger_gained
       << "\nWarmth: " << warmth_gained;

  if (type & EQUIPABLE)
    ss << "\nDefense: " << defense << "\nCold Resistance: " << cold_resist;

  if (type & TOOL)
    ss << "\nIt's a tool!";

  if (type & BLOCK)
    ss << "\nIt's a block!";

  return ss.str();
}

sf::Vector2f Item::getTexturePosition() { return texture_position; }

Item::Item()
    : name(""), texture_position(sf::Vector2f{0, 0}), stack_size(0),
      max_stack_size(0), type(0), hp_gained(0), hunger_gained(0),
      warmth_gained(0), defense(0), cold_resist(0), useAction(0),
      tooltipHover(0) {}

Item::~Item() {
  // intentionally left blank, nothing to destroy
}
