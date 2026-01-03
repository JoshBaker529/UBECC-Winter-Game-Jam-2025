#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "DeltaTime.hpp"
#include "Tilemap.hpp"
#include <SFML/Graphics.hpp>

class Tilemap; // Yes compiler, there IS a class called Tilemap.

class Entity {
protected:
  sf::Vector2f position, movement;
  sf::FloatRect boundingBox;

public:
  Entity(sf::Vector2f position, sf::Vector2f size) {
    this->position = position;
    this->boundingBox.position = position - (size / 2.f);
    this->boundingBox.size = size;
  };

  sf::Vector2f getPosition() { return position; }
  sf::Vector2f getMovement() { return movement; }
  sf::FloatRect getBoundingBox() { return boundingBox; }

  void setPosition(sf::Vector2f position) { this->position = position; }
  void setMovement(sf::Vector2f movement) { this->movement = movement; }
  void setBoundingBox(sf::FloatRect boundingBox) {
    this->boundingBox = boundingBox;
  }

  void recenterBoundingBox() {
    boundingBox.position = position - (boundingBox.size / 2.f);
  }

  bool testCollision(Tilemap &tilemap, sf::Vector2f position) {
    sf::Vector2f oldPosition = this->position;
    this->position = position;
    recenterBoundingBox();

    bool hit = tilemap.collisionRect(boundingBox);
    this->position = oldPosition;
    recenterBoundingBox();
    return hit;
  }

  void move(Tilemap &tilemap) {
    if (movement == sf::Vector2f(0.f, 0.f))
      return;
    sf::Vector2f movementCompensated = movement * DeltaTime::get();

    if (!testCollision(tilemap,
                       position + sf::Vector2f(movementCompensated.x, 0.f))) {
      position.x += movementCompensated.x;
      recenterBoundingBox();
    } else {
      movement.x = 0.f;
    }

    if (!testCollision(tilemap,
                       position + sf::Vector2f(0.f, movementCompensated.y))) {
      position.y += movementCompensated.y;
      recenterBoundingBox();
    } else {
      movement.y = 0.f;
    }
  }

  void move() {
    if (movement == sf::Vector2f(0.f, 0.f))
      return;
    sf::Vector2f movementCompensated = movement * DeltaTime::get();

    position.x += movementCompensated.x;
    position.y += movementCompensated.y;
    recenterBoundingBox();
  }

  void applyForce(sf::Vector2f force) { movement += force; }

  void applyResistance(float resistance) {
    if (movement == sf::Vector2f{0, 0})
      return;
    sf::Vector2f resistVector = (movement.normalized()) * resistance;
    sf::Vector2f result = movement - resistVector;

    if (movement.dot(result) < 0) {
      movement = {0, 0};
    } else {
      movement = result;
    }
  }

  void cap(float terminal) {
    if (movement.lengthSquared() <= (terminal * terminal))
      return;
    movement = movement.normalized() * terminal;
  }

  bool collision(Entity &other) {
    return (boundingBox.findIntersection(other.getBoundingBox()) !=
            std::nullopt);
  }

  virtual void step(sf::RenderWindow &, sf::View &, Tilemap &) {}
};

#endif
