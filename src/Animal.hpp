

#include "Entity.hpp"
#include "Tilemap.hpp"
#include <SFML/System/Angle.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdlib>
class Animal : public Entity {

private:
  sf::Angle angle;
  static inline sf::Clock clock;
  static inline float spawnInterval = 0;
  static inline float delayTime = 0;
  static inline sf::Clock delay;
  float turn;
  bool canMove;

public:
  float health = 100.f;
  
  Animal(sf::Vector2f position, sf::Vector2f size)
      : Entity(position, size), angle(sf::degrees(0)), turn(1), canMove(true) {
    clock.start();
  }

  void step(sf::RenderWindow &window, sf::View &view, sf::Texture &texture,
            Tilemap &world) {

    if (rand() % 100 == 1) {
      turn = -turn;
    }

    if (clock.getElapsedTime() > sf::seconds(4)) {
      canMove = !canMove;
      clock.reset();
      clock.start();
    }

    angle += sf::degrees((rand() % 2) * turn);

    sf::Vector2f target(1, 0);
    target = target.rotatedBy(angle);

    if (canMove) {

      const float dt = DeltaTime::get();
      const float momentum = .5f * dt, friction = .25f * dt, terminal = .4f;

      if (testCollision(world, position + target * momentum)) {
        angle += angle;
        angle = angle.wrapUnsigned();
      }
      applyForce(target * momentum);
      applyResistance(friction);
      cap(terminal);

      move(world);
    }

    // sf::RectangleShape rect;
    // rect.setPosition(getBoundingBox().position);
    // rect.setSize(getBoundingBox().size);
    // rect.setFillColor(sf::Color::Blue);

    // sf::RectangleShape rect2;
    // rect2.setPosition(target * 100.f + getBoundingBox().position);
    // rect2.setSize({10, 10});
    // rect2.setFillColor(sf::Color::Green);

    sf::Sprite sprite(texture);
    sprite.setPosition(position);
    sprite.setOrigin(sf::Vector2f(16.f, 48.f));
    sprite.setTextureRect(sf::IntRect({0, 320}, {32, 64}));
    Entity::submitSprite(sprite);

    // window.draw(rect);
    // window.draw(rect2);
  }

  static void stepAll(sf::RenderWindow &window, sf::View &view, Tilemap &world,
                      sf::Texture &texture) {

    const int MAXENEMIES = 10;

    if (delay.getElapsedTime().asSeconds() > delayTime) {

      if (clock.getElapsedTime().asSeconds() > spawnInterval) {
        if (all.size() < MAXENEMIES && rand()) {
          sf::Vector2f randomOffscreenPoint = view.getCenter();
          if (rand() % 2 == 1) {
            randomOffscreenPoint.x += view.getSize().x;
          } else {
            randomOffscreenPoint.x -= view.getSize().x;
          }

          if (rand() % 2 == 1) {
            randomOffscreenPoint.y += rand() % (int)(view.getSize().y / 2);
          } else {
            randomOffscreenPoint.y -= rand() % (int)(view.getSize().y / 2);
          }

          all.push_back(Animal(randomOffscreenPoint, sf::Vector2f{5.f, 5.f}));

          spawnInterval--;
          clock.restart();
        }
      }

      for (Animal &animal : all) {
        animal.step(window, view, texture, world);
      }
    }
  }
  static inline std::list<Animal> all;
};
