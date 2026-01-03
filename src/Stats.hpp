#pragma once

// This class holds all the information regarding the player stats

#include "Controls.hpp"
#include "SFML/Graphics/BlendMode.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Keyboard.hpp"
#include <iostream>

// Starting position
// constexpr sf::Vector2f STATS_START(0.f, -384.f + 10.f);
constexpr sf::Vector2f STATS_START(0.f, 384.f + 10.f);
// constexpr sf::Vector2f STATS_START(500.f, 500.f);
constexpr float BAR_HEIGHT = 30.f;
constexpr float BAR_WIDTH = 100.f;
constexpr float BUFFER = 20.f;
constexpr unsigned BACKGROUND_COLOR = 0x404040FF;
constexpr float BORDER = 5.f;
constexpr unsigned BAR_COLORS[]{0xFF4444FF, 0x44FF44FF, 0x4444FFFF};

class Stats {
private:
  float old_health;
  float old_hunger;
  float old_warmth;

public:
  Stats();
  Stats(Stats &&) = default;
  Stats(const Stats &) = default;
  Stats &operator=(Stats &&) = default;
  Stats &operator=(const Stats &) = default;
  ~Stats();
  void PrintStats();
  void draw(sf::RenderWindow &);

  float max_health;
  float max_hunger;
  float max_warmth;
  float health;
  float hunger;
  float warmth;
  float defense;
  float cold_resist;
  float time;

private:
  sf::VertexArray array;
};

static Stats PlayerStats;

Stats::Stats()
    : old_health(100), old_hunger(100), old_warmth(100), max_health(100),
      max_hunger(100), max_warmth(100), health(100), hunger(100), warmth(100),
      defense(0), cold_resist(0), time(0) {

  Controls::addButton(sf::Keyboard::Key::V);

  array = sf::VertexArray(sf::PrimitiveType::Triangles, 36);
  for (int i = 0; i < 3; i++) {
    sf::Vector2f UpLeft =
                     STATS_START + sf::Vector2f{(BAR_WIDTH + BUFFER) * i, 0},
                 UpRight = UpLeft + sf::Vector2f{BAR_WIDTH, 0},
                 DownLeft = UpLeft + sf::Vector2f{0, BAR_HEIGHT},
                 DownRight = UpLeft + sf::Vector2f{BAR_WIDTH, BAR_HEIGHT};

    int index_background = (i * 6);
    int index_foreground = (18 + index_background);

    array[index_background + 0].position = UpLeft;
    array[index_background + 1].position = UpRight;
    array[index_background + 2].position = DownRight;
    array[index_background + 3].position = DownRight;
    array[index_background + 4].position = DownLeft;
    array[index_background + 5].position = UpLeft;
    array[index_background + 0].color = sf::Color(BACKGROUND_COLOR);
    array[index_background + 1].color = sf::Color(BACKGROUND_COLOR);
    array[index_background + 2].color = sf::Color(BACKGROUND_COLOR);
    array[index_background + 3].color = sf::Color(BACKGROUND_COLOR);
    array[index_background + 4].color = sf::Color(BACKGROUND_COLOR);
    array[index_background + 5].color = sf::Color(BACKGROUND_COLOR);

    UpLeft += sf::Vector2f{BORDER, BORDER};
    UpRight += sf::Vector2f{-BORDER, BORDER};
    DownLeft += sf::Vector2f{BORDER, -BORDER};
    DownRight += sf::Vector2f{-BORDER, -BORDER};

    array[index_foreground + 0].position = UpLeft;
    array[index_foreground + 1].position = UpRight;
    array[index_foreground + 2].position = DownRight;
    array[index_foreground + 3].position = DownRight;
    array[index_foreground + 4].position = DownLeft;
    array[index_foreground + 5].position = UpLeft;
    array[index_foreground + 0].color = sf::Color(BAR_COLORS[i]);
    array[index_foreground + 1].color = sf::Color(BAR_COLORS[i]);
    array[index_foreground + 2].color = sf::Color(BAR_COLORS[i]);
    array[index_foreground + 3].color = sf::Color(BAR_COLORS[i]);
    array[index_foreground + 4].color = sf::Color(BAR_COLORS[i]);
    array[index_foreground + 5].color = sf::Color(BAR_COLORS[i]);
  }
}

void Stats::draw(sf::RenderWindow &window) {

  if (Controls::tapped(sf::Keyboard::Key::V)) {
    health -= 50;
    hunger -= 50;
    warmth -= 50;
  }

  int index = 18;
  if (health != old_health) {
    array[index + 1].position += sf::Vector2f{health - old_health, 0};
    array[index + 2].position += sf::Vector2f{health - old_health, 0};
    array[index + 3].position += sf::Vector2f{health - old_health, 0};
    old_health = health;
  }
  index += 6;
  if (hunger != old_hunger) {
    array[index + 1].position += sf::Vector2f{hunger - old_hunger, 0};
    array[index + 2].position += sf::Vector2f{hunger - old_hunger, 0};
    array[index + 3].position += sf::Vector2f{hunger - old_hunger, 0};
    old_hunger = hunger;
  }
  index += 6;
  if (warmth != old_warmth) {
    array[index + 1].position += sf::Vector2f{warmth - old_warmth, 0};
    array[index + 2].position += sf::Vector2f{warmth - old_warmth, 0};
    array[index + 3].position += sf::Vector2f{warmth - old_warmth, 0};
    old_warmth = warmth;
  }

  window.draw(array);
}

void Stats::PrintStats() {
  std::cout << "Health: " << health << "/" << max_health
            << "\nHunger: " << hunger << "/" << max_hunger
            << "\nWarmth: " << warmth << "/" << max_warmth
            << "\nDefense: " << defense << "\nCold Resist: " << cold_resist
            << std::endl;
}

Stats::~Stats() {}
