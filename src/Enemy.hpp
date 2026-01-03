#pragma once

#include "DeltaTime.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "Stats.hpp"
#include "Tilemap.hpp"
#include "Utilities.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <list>
#include <stack>
#include <unordered_map>
#include <unordered_set>

class Enemy : public Entity {
protected:
  std::list<sf::Vector2i> path;
  Entity *target = nullptr;

  sf::VertexArray body;
  float terminal;
  
  int frame = 0;
  

public:

  float health = 100.f;

  Enemy(sf::Vector2f position, sf::Vector2f size)
      : Entity(position, size), body(sf::PrimitiveType::Lines, 8) {

    sf::Vector2f curr = position;
    body[0].position = curr;

    curr.x += size.x;
    body[1].position = curr;
    body[2].position = curr;

    curr.y += size.y;
    body[3].position = curr;
    body[4].position = curr;

    curr.x -= size.x;
    body[5].position = curr;
    body[6].position = curr;

    curr.y -= size.y;
    body[7].position = curr;

    body[0].color = body[1].color = body[2].color = body[3].color =
        body[4].color = body[5].color = body[6].color = body[7].color =
            sf::Color::Blue;

    terminal = 1.f + randFloat(1, 2);
  }

  virtual void step(sf::RenderWindow &window, sf::View &view, Tilemap &world,
                    bool draw) {}

  void setTarget(Entity &tar) { target = &tar; }

  void clearTarget() { target = nullptr; }

	// bool collision(sf::FloatRect bb){
        // return (boundingBox.findIntersection(bb) != std::nullopt);
    // }

protected:
  struct Vector2iHash {
    size_t operator()(const sf::Vector2i &in) const {
      size_t x = std::hash<int>()(in.x), y = std::hash<int>()(in.y);
      return y ^ (x << 1);
    }
  };

  struct info {
    float gScore, fScore;
    sf::Vector2f prev;
  };

  struct Score {
    sf::Vector2f pos;
    float gScore;

    bool operator==(const Score &other) const { return pos == other.pos; }
  };

  void aStar(const sf::Vector2i &target, Tilemap &world) {

    const sf::Vector2i TILESIZE = {1, 1};

    const sf::Vector2i offsets[4] = {
        {TILESIZE.x, 0}, {-TILESIZE.x, 0}, {0, TILESIZE.y}, {0, -TILESIZE.y}};
    // TILESIZE,
    // {TILESIZE.x, -TILESIZE.y},
    // {-TILESIZE.x, TILESIZE.y},
    // {-TILESIZE.x, -TILESIZE.y}};

    std::unordered_map<sf::Vector2i, float, Vector2iHash> fScore;
    std::unordered_map<sf::Vector2i, float, Vector2iHash> gScore;
    std::unordered_map<sf::Vector2i, sf::Vector2i, Vector2iHash> prev;
    std::unordered_set<sf::Vector2i, Vector2iHash> toBeChecked;
    std::unordered_set<sf::Vector2i, Vector2iHash> visited;

    sf::Vector2i pos = world.worldToGridCoords(boundingBox.position);
    gScore[pos] = 0;
    fScore[pos] = dist(pos, target);

    toBeChecked.insert(pos);
    bool found = false;

    while (!toBeChecked.empty()) {
      sf::Vector2i curr = *toBeChecked.begin();
      float lowest = fScore[curr];
      for (auto it = ++toBeChecked.begin(); it != toBeChecked.end(); ++it) {
        if (fScore[*it] < lowest) {
          curr = *it;
          lowest = fScore[curr];
        }
      }

      if (curr == target) {
        found = true;
        break;
      }

      toBeChecked.erase(curr);
      visited.insert(curr);

      // std::cout << curr.x << " " << curr.y << '\n';
      for (const sf::Vector2i &offset : offsets) {
        sf::Vector2i neighbor = curr + offset;
        if (world.getTileAtIndex(neighbor)->type == Tilemap::Tile::solid) {
          continue;
        }

        float tempGScore = gScore[curr] + dist(curr, neighbor);

        if (tempGScore > 10)
          continue;

        if (gScore.count(neighbor) == 0 || tempGScore < gScore[neighbor]) {
          prev[neighbor] = curr;
          gScore[neighbor] = tempGScore;
          fScore[neighbor] = tempGScore + dist(neighbor, target);

          if (toBeChecked.count(neighbor) == 0 &&
              visited.count(neighbor) == 0) {
            toBeChecked.insert(neighbor);
          }
        }
      }
    }

    if (!found) {
      return;
    }

    sf::Vector2i curr = target;
    while (curr != pos) {

      path.push_back(curr);
      curr = prev.at(curr);
    }
  }

  float dist(const sf::Vector2i &p1, const sf::Vector2i &p2) {
    return pow(fabs(p1.x - p2.x), 2) + pow(fabs(p1.y - p2.y), 2);
  }
};

class regularSnowman :public  Enemy {
public:
  regularSnowman(sf::Vector2f position) : Enemy(position, {24, 24}) {}
  void step(sf::RenderWindow &window, sf::View &view, Tilemap &world, sf::Texture &texture) {
    // static sf::VertexArray temp(sf::PrimitiveType::LineStrip);

    const float dt = DeltaTime::get();
    const float DUMDDISTANCE = 100.f;
    const float WARMTHDAMAGE = .1f;
    if (target != nullptr) {

      const float momentum = .5f * dt, friction = .25f * dt;

      if (path.empty() &&
          dist(world.worldToGridCoords(target->getPosition()),
               world.worldToGridCoords(boundingBox.position)) > DUMDDISTANCE) {
        sf::Vector2f distTo = target->getPosition() - position;
        applyForce(distTo.normalized() * momentum);
        applyResistance(friction);
        cap(terminal);
        move(world);

      } else {
        path.clear();
          aStar(world.worldToGridCoords(target->getPosition()), world);

        // temp.clear();
        if (!path.empty()) {
          // for (auto it = path.begin(); it != path.end(); ++it) {
          // sf::Vector2f cord = {
          // it->x * world.getTileSize().x + world.getTileSize().x / 2,
          // it->y * world.getTileSize().y + world.getTileSize().y / 2};
          // temp.append({cord, sf::Color::Green});
          // }

          sf::Vector2f distTo =
              sf::Vector2f{path.back().x * world.getTileSize().x +
                               world.getTileSize().x / 2,
                           path.back().y * world.getTileSize().y +
                               world.getTileSize().y / 2} -
              position;

          if (distTo.length() < world.getTileSize().x) {
            path.pop_back();
          }

          if (!path.empty()) {
            sf::Vector2f distTo =
                sf::Vector2f{path.back().x * world.getTileSize().x +
                                 world.getTileSize().x / 2,
                             path.back().y * world.getTileSize().y +
                                 world.getTileSize().y / 2} -
                position;

            if (distTo != sf::Vector2f{0, 0}) {
              applyForce(distTo.normalized() * momentum);
              applyResistance(friction);
              cap(terminal);

              move(world);
            }
          }
        }
        // path.pop();
      }
    }

    if (!Player::all.empty() && collision(Player::all.front())) {
      StatsContainer::stats.warmth -= WARMTHDAMAGE * dt;
      if (StatsContainer::stats.warmth < 0)
        StatsContainer::stats.warmth = 0;
    }

	if( movement != sf::Vector2f{0.f,0.f} ){
		frame = 0;
		float angle = ((movement).angle()+sf::degrees(45/2)).wrapUnsigned().asDegrees();
		frame += (int)(angle / 45);
	}

	sf::Sprite sprite(texture);
    sprite.setPosition(position);
    sprite.setOrigin(sf::Vector2f(16.f, 48.f));
    sprite.setTextureRect(sf::IntRect({frame*32, 128}, {32, 64}));
    Entity::submitSprite(sprite);

    // sf::RectangleShape rect;
    // rect.setPosition(getBoundingBox().position);
    // rect.setSize(getBoundingBox().size);
    // rect.setFillColor(sf::Color::Blue);
    // window.draw(rect);
    // window.draw(temp);
  }

  static void stepAll(sf::RenderWindow &window, sf::View &view,
                      Tilemap &world, sf::Texture &texture) {
						  
		list<typename list<regularSnowman>::iterator> deadThings;
		

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

          all.push_back(regularSnowman(randomOffscreenPoint));

          if (!Player::all.empty())
            all.back().setTarget(Player::all.front());
        }
        spawnInterval--;
        clock.restart();
      }
    }

    for (auto i = all.begin(); i != all.end(); i++) {
      i->step(window, view, world, texture);
	  if (i->dead) deadThings.push_back(i);
    }
	for (auto i = deadThings.begin(); i != deadThings.end(); i++)
			all.erase(*i);
  }
  static inline float spawnInterval = 30;
  static inline float delayTime = 100;
  static inline sf::Clock delay;
  static inline sf::Clock clock;
  static inline std::list<regularSnowman> all;
};

class ghostSnowman : public Enemy {
public:
  ghostSnowman(sf::Vector2f position) : Enemy(position, {24, 24}) {}
  void step(sf::RenderWindow &window, sf::View &view, Tilemap &world, sf::Texture &texture) {

    const float dt = DeltaTime::get();
    const float momentum = .5f * dt, friction = .25f * dt;
    const float WARMTHDAMAGE = .1f;

    sf::Vector2f distTo = target->getPosition() - position;
    applyForce(distTo.normalized() * momentum);
    applyResistance(friction);
    cap(terminal);
    move();
    if (!Player::all.empty() && collision(Player::all.front())) {
      StatsContainer::stats.warmth -= WARMTHDAMAGE * dt;
      if (StatsContainer::stats.warmth < 0)
        StatsContainer::stats.warmth = 0;
    }
	
	if( movement != sf::Vector2f{0.f,0.f} ){
		frame = 0;
		float angle = ((movement).angle()+sf::degrees(45/2)).wrapUnsigned().asDegrees();
		frame += (int)(angle / 45);
	}

	sf::Sprite sprite(texture);
    sprite.setPosition(position);
	sprite.setColor(sf::Color(255,255,255,128));
    sprite.setOrigin(sf::Vector2f(16.f, 48.f));
    sprite.setTextureRect(sf::IntRect({frame*32, 128}, {32, 64}));
    Entity::submitSprite(sprite);
	
    // sf::RectangleShape rect;
    // rect.setPosition(getBoundingBox().position);
    // rect.setSize(getBoundingBox().size);
    // rect.setFillColor(sf::Color::Blue);
    // window.draw(rect);
  }

  static void stepAll(sf::RenderWindow &window, sf::View &view,
                      Tilemap &world, sf::Texture &texture) {
						  
						  		list<typename list<ghostSnowman>::iterator> deadThings;

    const int MAXENEMIES = 10;
    const int SPAWNCHANCE = 200;

    float STARTSECONDS = 300;
    float increase = 60;

    if (delay.getElapsedTime().asSeconds() > delayTime) {

      if (clock.getElapsedTime().asSeconds() > spawnInterval) {
        if (all.size() < MAXENEMIES) {
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
          all.push_back(ghostSnowman(randomOffscreenPoint));

          if (!Player::all.empty())
            all.back().setTarget(Player::all.front());
        }
        spawnInterval--;
        clock.restart();
      }
    }

	for (auto i = all.begin(); i != all.end(); i++) {
      i->step(window, view, world, texture);
	  if (i->dead) deadThings.push_back(i);
    }
	for (auto i = deadThings.begin(); i != deadThings.end(); i++)
			all.erase(*i);
	
  }

  static inline float spawnInterval = 150;
  static inline float delayTime = 900;
  static inline sf::Clock delay;
  static inline sf::Clock clock;
  static inline std::list<ghostSnowman> all;
};
