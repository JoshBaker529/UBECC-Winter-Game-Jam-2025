#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Campfire.hpp"
#include "Controls.hpp"
#include "DeltaTime.hpp"
#include "Entity.hpp"
#include "Inventory.hpp"
#include "Particles.hpp"
#include "Stats.hpp"
#include "Tilemap.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <list>
using std::list;

class Entity;
class Tilemap;

class Player : public Entity {
public:
  Player(sf::Vector2f position) : Entity(position, sf::Vector2f(24, 24)) {}

  virtual void step(sf::RenderWindow &window, sf::View &view,
                    sf::Texture &texture, Tilemap &tilemap) {
    const float dt = DeltaTime::get();
    const float momentum = 0.5f * dt, friction = 0.25f * dt, terminal = 4.f;
    const float COLDDAMAGE = .1f;
    const float WINDDAMAGE = .05f;
    const float HUNGERDAMAGE = .01f;
    const float PASSIVEHEALING = .1f;

    // Input
    sf::Vector2f input;
    input.x = (float)Controls::held(sf::Keyboard::Key::D) -
              (float)Controls::held(sf::Keyboard::Key::A);
    input.y = (float)Controls::held(sf::Keyboard::Key::S) -
              (float)Controls::held(sf::Keyboard::Key::W);
    if (input != sf::Vector2f{0, 0})
      input = input.normalized();

    // Movement
    applyForce(input * momentum);
    applyResistance(friction);
    cap(terminal);
    move(tilemap);
    bound();

    // Near a campfire
    bool nearCampfire = false;
    float nearDist = 128.f;
    for (auto i = Campfire::all.begin(); i != Campfire::all.end(); i++) {
      if ((i->getPosition() - position).lengthSquared() <= pow(nearDist, 2)) {
        nearCampfire = true;
        break;
      }
    }

    if (Particles::positionIsExposed(position, tilemap)) {
      StatsContainer::stats.warmth -= WINDDAMAGE * dt;
    }

    if (nearCampfire) {
      StatsContainer::stats.warmth += (WINDDAMAGE + .1f) * dt;
    }

    StatsContainer::stats.hunger -= HUNGERDAMAGE * dt;

    if (StatsContainer::stats.warmth > StatsContainer::stats.max_warmth / 2 &&
        StatsContainer::stats.hunger > StatsContainer::stats.max_hunger / 2) {
      StatsContainer::stats.health += PASSIVEHEALING * dt;
      if (StatsContainer::stats.health > StatsContainer::stats.max_health) {

        StatsContainer::stats.health = StatsContainer::stats.max_health;
      }
    }

    // Move Camera
    view.move((position - view.getCenter()) / 1.f);
    window.setView(view);

    // sf::RectangleShape r;
    // r.setPosition(boundingBox.position);
    // r.setSize(boundingBox.size);
    // r.setFillColor(sf::Color::Red);
    // window.draw(r);

    if (StatsContainer::stats.warmth <= 0) {
      StatsContainer::stats.health -= COLDDAMAGE * dt;
    }
    // Draw Code
    sf::Sprite sprite(texture);
    sprite.setPosition(position);
    sprite.setOrigin(sf::Vector2f(16.f, 48.f));
    sprite.setTextureRect(sf::IntRect({0, 224}, {32, 64}));
    Entity::submitSprite(sprite);
  }

  static inline Inventory inventory;

  static inline list<Player> all;
};

#endif
