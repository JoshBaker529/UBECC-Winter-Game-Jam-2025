#ifndef STATES_HPP
#define STATES_HPP

#include "Background.hpp"
#include "Campfire.hpp"
#include "Controls.hpp"
#include "Decor.hpp"
#include "DeltaTime.hpp"
#include "Enemy.hpp"
#include "Entity.hpp"
#include "Inventory.hpp"
#include "Message.hpp"
#include "Particles.hpp"
#include "Pickup.hpp"
#include "Plant.hpp"
#include "Player.hpp"
#include "Procedural.hpp"
#include "Stats.hpp"
#include "Tilemap.hpp"
#include "Animal.hpp"
#include "Sword.hpp"


#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/WindowEnums.hpp>

#include <iostream>
using std::cout;
using std::endl;

class MainState;

class State {
public:
  virtual State *loop(MainState *) = 0;
  virtual ~State() {};
};

class Pause : public State {
private:
  sf::RenderWindow &window;
  sf::View &view;
  sf::View &guiView;

public:
  ~Pause() {}
  Pause(sf::RenderWindow &window, sf::View &view, sf::View &guiView)
      : window(window), view(view), guiView(guiView) {}
  State *loop(MainState *mainState);
};

class MainState : public State {
private:
  sf::RenderWindow window;
  sf::View view;
  float zoom = 0.5f;
  sf::View guiView;
  sf::Clock clock;

  sf::Texture frontTileset;
  sf::Texture backTileset;
  sf::Texture characterTexture;
  sf::Texture itemTexture;
  sf::Texture shadowTexture;
  sf::Texture fogTexture;
  sf::RectangleShape nightRect;
  sf::Font font;

  Background background;
  Background shadows;

  Tilemap midTiles;
  Tilemap frontTiles;
  Tilemap::Tile wall;

  sf::Angle windAngle;

public:
  ~MainState() {}
  MainState()
      : window(sf::VideoMode({1024, 768}), ""),
        background(sf::Vector2i(100, 100), sf::Vector2f(32.f, 32.f),
                   sf::Vector2f(96.f, 96.f), sf::Vector2f(-32.f, -32.f)),
        shadows(sf::Vector2i(100, 100), sf::Vector2f(32.f, 32.f),
                sf::Vector2f(96.f, 96.f), sf::Vector2f(-32.f, -32.f)),
        midTiles(sf::Vector2f(32.f, 32.f), sf::Vector2f(32.f, 32.f)),
        frontTiles(sf::Vector2f(32.f, 32.f), sf::Vector2f(32.f, 32.f)),
        windAngle(sf::degrees(0.f)) {

    window.setVerticalSyncEnabled(true);

    view.setSize((sf::Vector2f)window.getSize());
    view.setCenter(sf::Vector2f(0, 0));
    view.zoom(zoom);

    guiView.setCenter(((sf::Vector2f)window.getSize()) / 2.f);

    if (!frontTileset.loadFromFile("res/frontTiles.png"))
      exit(1);
    if (!frontTileset.generateMipmap())
      exit(1);

    if (!backTileset.loadFromFile("res/backTiles.png"))
      exit(1);
    if (!backTileset.generateMipmap())
      exit(1);

    if (!characterTexture.loadFromFile("res/characters.png"))
      exit(1);
    if (!characterTexture.generateMipmap())
      exit(1);

    if (!itemTexture.loadFromFile("res/items.png"))
      exit(1);
    if (!itemTexture.generateMipmap())
      exit(1);

    if (!shadowTexture.loadFromFile("res/shadow.png"))
      exit(1);
    if (!shadowTexture.generateMipmap())
      exit(1);

    if (!fogTexture.loadFromFile("res/fog.png"))
      exit(1);

    nightRect.setFillColor(sf::Color::Blue);

    if (!font.openFromFile("res/tuffy.ttf"))
      exit(1);

    midTiles.render();

    frontTiles.setOffset(sf::Vector2f(0.f, -32.f));
    frontTiles.render();

    wall.type = Tilemap::Tile::Type::solid;
    wall.vanity = sf::Vector2i(3, 2);

    // Controls
    Controls::addButton(sf::Keyboard::Key::W);
    Controls::addButton(sf::Keyboard::Key::A);
    Controls::addButton(sf::Keyboard::Key::S);
    Controls::addButton(sf::Keyboard::Key::D);
    Controls::addButton(sf::Mouse::Button::Left);
    Controls::addButton(sf::Mouse::Button::Right);

    Controls::addButton(sf::Keyboard::Key::LControl);
    Controls::addButton(sf::Keyboard::Key::Escape);

    Controls::addButton(sf::Keyboard::Key::Num1);
    Controls::addButton(sf::Keyboard::Key::Num2);
    Controls::addButton(sf::Keyboard::Key::Num3);
    Controls::addButton(sf::Keyboard::Key::Num4);
    Controls::addButton(sf::Keyboard::Key::Num5);
    Controls::addButton(sf::Keyboard::Key::C);
    Controls::addButton(sf::Keyboard::Key::E);
    Controls::addButton(sf::Keyboard::Key::F);
	
    Controls::addButton(sf::Keyboard::Key::R);

    Controls::windowRelativeTo(window);

    

    // Campfire::all.push_back(Campfire({100.f, 100.f}));
    // Plant::all.push_back(
        // Plant({1600.f + 50.f, 1600.f + 50.f}, Plant::Type::Tree));
    // Plant::all.push_back(
        // Plant({1600.f + 150.f, 1600.f + 50.f}, Plant::Type::DeadBush));
    // Plant::all.push_back(
        // Plant({1600.f + 250.f, 1600.f + 50.f}, Plant::Type::BerryBush));
    // Plant::all.push_back(
        // Plant({1600.f + 350.f, 1600.f + 50.f}, Plant::Type::Grass));
    // Plant::all.push_back(
        // Plant({1600.f + 450.f, 1600.f + 50.f}, Plant::Type::Carrot));

    
    // // Campfire::all.push_back(Campfire({100.f, 100.f}));
    // Plant::all.push_back(
        // Plant({1600.f + 50.f, 1600.f + 50.f}, Plant::Type::Grass));
		
		Noise::generate();
		Procedural::generateLevel(background, shadows, midTiles, frontTiles);
		Player::all.push_back(Player({1600.f, 1600.f}));
  }

  void drawBackground() {
    background.draw(window, backTileset);
    shadows.draw(window, shadowTexture);
    midTiles.draw(window, frontTileset);
  }

  void drawForeground() { frontTiles.draw(window, frontTileset); }

  State *loop(MainState *) {

    while (window.isOpen()) {
      DeltaTime::set(clock.getElapsedTime().asSeconds() / (1.f / 60.f));
      if (DeltaTime::get() > 2.f)
        DeltaTime::set(2.f);
      clock.restart();

      while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
          window.close();
        else if (const auto *resized = event->getIf<sf::Event::Resized>()) {
          view.setSize((sf::Vector2f)resized->size);
          view.zoom(zoom);
        } else if (const auto *mouseWheelScrolled =
                       event->getIf<sf::Event::MouseWheelScrolled>()) {

          if (Controls::held(sf::Keyboard::Key::LControl)) {
            zoom += (mouseWheelScrolled->delta == 1) ? -0.1f : 0.1f;
            if (zoom > 1.f)
              zoom = 1.f;
            if (zoom < 0.5f)
              zoom = 0.5f;
            view.setSize((sf::Vector2f)window.getSize());
            view.zoom(zoom);
          } else {
            if (mouseWheelScrolled->delta == 1) {
              Player::inventory.moveHotbarLeft();
            } else {
              Player::inventory.moveHotbarRight();
            }
          }
        }
      }

      if (window.hasFocus()) {
        Controls::updateKeys();
        Controls::updateMouse();

        if (Controls::tapped(sf::Keyboard::Key::Escape)) {
          return new Pause(window, view, guiView);
        }
		
		if( Player::all.empty() && Controls::tapped(sf::Keyboard::Key::R)){
			midTiles.clear();
			frontTiles.clear();
			background.clear();
			shadows.clear();
			
			Noise::generate();
			Procedural::generateLevel(background, shadows, midTiles, frontTiles);
			Player::all.push_back(Player({1600.f, 1600.f}));
			regularSnowman::all.clear();
			ghostSnowman::all.clear();
			Animal::all.clear();
			Decor::all.clear();
			Plant::all.clear();
			Campfire::all.clear();
			Pickup::all.clear();
			
			StatsContainer::stats.health = StatsContainer::stats.max_health; 
			StatsContainer::stats.hunger = StatsContainer::stats.max_hunger; 
			StatsContainer::stats.warmth = StatsContainer::stats.max_warmth; 
		}

        window.clear(sf::Color::White);
        window.setView(view);
        // Main Loop code below here:

        sf::Vector2f mousePosition = (sf::Vector2f)window.mapPixelToCoords(
            sf::Mouse::getPosition(window));
        sf::Vector2f ts = midTiles.getTileSize();

        // Object step()'s here
        Entity::stepAll<Player>(Player::all, window, view, characterTexture,
                                frontTiles);
        Entity::stepAll<Decor>(Decor::all, window, view, characterTexture,
                               frontTiles);
        Entity::stepAll<Pickup>(Pickup::all, window, view, itemTexture,
                                frontTiles);
        Entity::stepAll<Campfire>(Campfire::all, window, view, characterTexture,
                                  frontTiles);
        Entity::stepAll<Plant>(Plant::all, window, view, characterTexture,
                               frontTiles);       
		Entity::stepAll<Animal>(Animal::all, window, view, characterTexture,
                               frontTiles);
		Entity::stepAll<Sword>(Sword::all, window, view, itemTexture,
                               frontTiles);

        // Make Snow
        Particles::snow(view);
        windAngle += sf::degrees(0.05f * DeltaTime::get());
        Particles::setWindDirection(windAngle);

		StatsContainer::stats.time += (1.f/60.f) * DeltaTime::get();

        drawBackground();

        // Object draw()'s here
        Entity::drawAll(window);
        Particles::draw(window, midTiles);

		Item hb = Player::inventory.getHotbarItem();

        // If the player is alive, do block stuffs
        if (Player::all.size() > 0) {
          Player *player = &Player::all.front();
          bool inRange =
              abs(mousePosition.x - player->getPosition().x) <= 128.f;
          inRange = inRange &&
                    abs(mousePosition.y - player->getPosition().y) <= 128.f;

          sf::Vector2f fp(floor(mousePosition.x / ts.x),
                          floor(mousePosition.y / ts.y));
          fp *= ts.x;

          sf::RectangleShape blockCursor;
          blockCursor.setPosition(fp);
          blockCursor.setFillColor(sf::Color::Transparent);
          blockCursor.setOutlineThickness(1);
          blockCursor.setOutlineColor(sf::Color(255, 255, 255, 128));
          blockCursor.setSize(ts);

          sf::FloatRect blockRect{fp, ts};
          bool tooClose = blockRect.findIntersection(
                              player->getBoundingBox()) != std::nullopt;
          if (tooClose)
            blockCursor.setOutlineColor(sf::Color(255, 0, 0, 128));

          if (inRange) {
            window.draw(blockCursor);
            blockCursor.move({0.f, -ts.y});
            window.draw(blockCursor);
          }

          if (Controls::tapped(sf::Mouse::Button::Left) && hb.getName() == "Sword" && Sword::all.empty()) {
			  Sword::all.push_back( Sword() );
		  }

          if (inRange && Controls::tapped(sf::Mouse::Button::Left)) {
            if (midTiles.getTile(mousePosition)->type !=
                    Tilemap::Tile::Type::air &&
                hb.getName() == "Pickaxe") {

              shadows.setTile(mousePosition, 0);

              shadows.render();
              midTiles.removeTile(mousePosition);
              midTiles.render();

              frontTiles.removeTile(mousePosition);
              frontTiles.autoTile();
              frontTiles.render();

              sf::Vector2f center = fp + (ts / 2.f);
              Pickup::all.push_back(Pickup(center, "Stone"));
              Particles::burst({center.x, center.y, 1.f},
                               sf::Color(64, 64, 64));
            } else {
              for (auto i = Plant::all.begin(); i != Plant::all.end(); i++) {
                sf::FloatRect bb = i->getBoundingBox();
                if (bb.contains(mousePosition)) {
                  switch (i->type) {
                  case Plant::Type::Tree:
					if( hb.getName() == "Axe" ){
						Particles::burst({mousePosition.x, mousePosition.y, 1.f},
										 sf::Color(115, 97, 75));
						for (int i = 0; i < 10; i++)
						  Pickup::all.push_back(Pickup(mousePosition, "Stick"));
						i->kill();
					}
                    break;
                  case Plant::Type::DeadBush:
                    Particles::burst({mousePosition.x, mousePosition.y, 1.f},
                                     sf::Color(115, 97, 75));
                    Pickup::all.push_back(Pickup(mousePosition, "Stick"));
                    i->kill();
                    break;
                  case Plant::Type::BerryBush:
                    Particles::burst({mousePosition.x, mousePosition.y, 1.f},
                                     sf::Color(115, 97, 75));
                    Pickup::all.push_back(Pickup(mousePosition, "Berries"));
                    i->kill();
                    break;
                  case Plant::Type::Grass:
                    Particles::burst({mousePosition.x, mousePosition.y, 1.f},
                                     sf::Color(99, 153, 129));
                    Pickup::all.push_back(Pickup(mousePosition, "Grass"));
                    i->kill();
                    break;
                  case Plant::Type::Carrot:
                    Particles::burst({mousePosition.x, mousePosition.y, 1.f},
                                     sf::Color(99, 153, 129));
                    Pickup::all.push_back(Pickup(mousePosition, "Carrot"));
                    i->kill();
                    break;
                  }
                }
              }
            }
          }
          if ((inRange && !tooClose) &&
              Controls::tapped(sf::Mouse::Button::Right)) {
            if (hb.getName() == "Stone") {
              shadows.setTile(mousePosition, 1);
              shadows.render();

              midTiles.addTile(mousePosition, wall);
              midTiles.render();

              frontTiles.addTile(mousePosition, wall);
              frontTiles.autoTile();
              frontTiles.render();

              Player::inventory.decHotbarItem();
            }
            if (hb.getName() == "Campfire") {
              Campfire::all.push_back(Campfire(mousePosition));
              Player::inventory.decHotbarItem();
            }
          }
          if ((inRange && !tooClose) &&
              Controls::tapped(sf::Keyboard::Key::C)) {
          }
        }
		
		if( Controls::tapped(sf::Mouse::Button::Right) ){
			if (hb.getActionFunction()) {
				hb.useActionFunction();
				Player::inventory.decHotbarItem();
			  }
		}

        regularSnowman::stepAll(window, view, midTiles, characterTexture);
        ghostSnowman::stepAll(window, view, midTiles, characterTexture);

        // Draw foreground stuff
        drawForeground();

        guiView.setCenter(((sf::Vector2f)window.getSize()) / 2.f);
        guiView.setSize((sf::Vector2f)window.getSize());
        window.setView(guiView);

        // fogSprite.setPosition( -((sf::Vector2f)window.getSize()/2.f) );
        sf::Sprite fogSprite(fogTexture, {{0, 0}, {1600, 900}});
        fogSprite.setColor(sf::Color(255, 255, 255, 255));
        fogSprite.setScale(
            ((sf::Vector2f)window.getSize())
                .componentWiseDiv(fogSprite.getLocalBounds().size));
        window.draw(fogSprite);

        Player::inventory.draw(window);
        StatsContainer::stats.draw(window);

        Message::draw(window, guiView, font);

        sf::Text gameOverText(font);
		gameOverText.setPosition( ((sf::Vector2f)window.getSize())/2.f );
        gameOverText.setFillColor(sf::Color::White);
        gameOverText.setOutlineColor(sf::Color::Black);
        gameOverText.setString("Chris got lost - Press R to Respawn.");
        gameOverText.setCharacterSize(30);
        gameOverText.setStyle(sf::Text::Bold);
        gameOverText.setOrigin(gameOverText.getLocalBounds().size / 2.f);
        if (Player::all.size() <= 0)
          window.draw(gameOverText);
      }

      window.display();
    }

    return nullptr;
  }
};

State *Pause::loop(MainState *mainState) {

  sf::RectangleShape overlay;
  overlay.setSize(window.getView().getSize());
  overlay.setPosition(window.getView().getCenter() - overlay.getSize() / 2.f);
  overlay.setFillColor({255, 255, 255, 50});
  while (window.isOpen()) {

    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();
      else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scan::Escape) {
          return mainState;
        }
      }
      // else if (const auto* resized = event->getIf<sf::Event::Resized>())
      // {
      // view.setSize((sf::Vector2f)resized->size);
      // view.zoom(zoom);
      // }
    }
    if (window.hasFocus()) {
      window.clear();
      window.setView(view);
      mainState->drawBackground();
      mainState->drawForeground();

      guiView.setSize(view.getSize());
      window.setView(guiView);
      window.draw(overlay);

      window.display();
    }
  }
  return nullptr;
}
#endif
