

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
  sf::Clock clock;
  float turn;
  bool canMove;

public:
  Animal(sf::Vector2f position, sf::Vector2f size)
      : Entity(position, size), angle(sf::degrees(0)), turn(1), canMove(true) {
    clock.start();
  }

  void step(sf::RenderWindow &window, sf::View &view, Tilemap &world) {

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
      const float momentum = .5f * dt, friction = .25f * dt, terminal = 4.f;

      if (testCollision(world, position + target * momentum)) {
        angle += angle;
        angle = angle.wrapUnsigned();
      }
      cout << "Angle " << angle.asDegrees() << '\n';
      cout << "Target " << target.x << ' ' << target.y << "\n\n";
      applyForce(target * momentum);
      applyResistance(friction);
      cap(terminal);

      move(world);
    }

    sf::RectangleShape rect;
    rect.setPosition(getBoundingBox().position);
    rect.setSize(getBoundingBox().size);
    rect.setFillColor(sf::Color::Blue);

    sf::RectangleShape rect2;
    rect2.setPosition(target * 100.f + getBoundingBox().position);
    rect2.setSize({10, 10});
    rect2.setFillColor(sf::Color::Green);

    window.draw(rect);
    window.draw(rect2);
  }
};
