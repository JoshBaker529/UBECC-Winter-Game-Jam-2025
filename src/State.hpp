#ifndef STATES_HPP
#define STATES_HPP

#include "Background.hpp"
#include "Controls.hpp"
#include "Decor.hpp"
#include "Player.hpp"
#include "Procedural.hpp"
#include "Tilemap.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/WindowEnums.hpp>

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
  sf::View guiView;
  sf::Clock clock;

  // Textures
  sf::Texture frontTileset;
  sf::Texture backTileset;
  sf::Texture characterTexture;
  sf::Texture shadowTexture;
  sf::Font font;

  // Tilemaps

  Background background;
  Background shadows;

  Tilemap midTiles;
  Tilemap frontTiles;
  Tilemap::Tile wall;

  sf::Text gameOverText;

public:
  ~MainState() {}
  MainState()
      : window(sf::VideoMode({1024, 768}), ""),
        background{sf::Vector2i(100, 100), sf::Vector2f(32.f, 32.f),
                   sf::Vector2f(96.f, 96.f), sf::Vector2f(-32.f, -32.f)},
        shadows(sf::Vector2i(100, 100), sf::Vector2f(32.f, 32.f),
                sf::Vector2f(96.f, 96.f), sf::Vector2f(-32.f, -32.f)),
        midTiles(sf::Vector2f(32.f, 32.f), sf::Vector2f(32.f, 32.f)),
        frontTiles(sf::Vector2f(32.f, 32.f), sf::Vector2f(32.f, 32.f)),
        gameOverText(font) {

    window.setVerticalSyncEnabled(true);
    view.setSize((sf::Vector2f)window.getSize());
    view.setCenter(sf::Vector2f(0, 0));
    guiView.setCenter(sf::Vector2f(0, 0));
    if (!frontTileset.loadFromFile("res/frontTiles.png"))
      return;
    if (!frontTileset.generateMipmap())
      return;
    if (!backTileset.loadFromFile("res/backTiles.png"))
      return;
    if (!backTileset.generateMipmap())
      return;
    if (!characterTexture.loadFromFile("res/characters.png"))
      return;
    if (!characterTexture.generateMipmap())
      return;
    if (!shadowTexture.loadFromFile("res/shadow.png"))
      return;
    if (!shadowTexture.generateMipmap())
      return;
    if (!font.openFromFile("res/tuffy.ttf"))
      return;
    frontTiles.setOffset(sf::Vector2f(0.f, -32.f));
    frontTiles.render();
    midTiles.render();

    wall.type = Tilemap::Tile::Type::solid;
    wall.vanity = sf::Vector2i(3, 2);

    // Controls
    Controls::addButton(sf::Keyboard::Key::W);
    Controls::addButton(sf::Keyboard::Key::A);
    Controls::addButton(sf::Keyboard::Key::S);
    Controls::addButton(sf::Keyboard::Key::D);
    Controls::addButton(sf::Keyboard::Key::P);

    Noise::generate();
    Procedural::generateLevel(background, frontTiles);
    Player::all.push_back(Player());
    Decor::all.push_back(Decor({100.f, 50.f}, {24.f, 24.f}, {32, 80},
                               sf::IntRect{{32, 0}, {64, 96}}));
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setOutlineColor(sf::Color::Black);
    gameOverText.setString("GAME OVER - Press R to Respawn.");
    gameOverText.setCharacterSize(30);
    gameOverText.setStyle(sf::Text::Bold);
    gameOverText.setOrigin(gameOverText.getLocalBounds().size / 2.f);
  }

  void drawBackground() {

    // Draw background stuff
    background.draw(window, backTileset);
    shadows.draw(window, shadowTexture);
    midTiles.draw(window, frontTileset);

    // Object draw()'s here
    Entity::drawAll(window);
    Particles::draw(window);

    Entity::stepAll<Decor>(Decor::all, window, view, characterTexture,
                           frontTiles);
  }

  void drawForground() { frontTiles.draw(window, frontTileset); }

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
        } else if (const auto *mouseWheelScrolled =
                       event->getIf<sf::Event::MouseWheelScrolled>()) {
          view.zoom(mouseWheelScrolled->delta == 1 ? 0.9f : (1 / 0.9f));
        } else if (const auto *keyPressed =
                       event->getIf<sf::Event::KeyPressed>()) {
          if (keyPressed->scancode == sf::Keyboard::Scan::R) {
            if (!frontTileset.loadFromFile("res/frontTiles.png"))
              return nullptr;
            if (!frontTileset.generateMipmap())
              return nullptr;
          } else if (keyPressed->scancode == sf::Keyboard::Scan::G) {
            Procedural::generateLevel(background, frontTiles);
            background.render();
          } else if (keyPressed->scancode == sf::Keyboard::Scan::P) {
            return new Pause(window, view, guiView);
          }
        }
      }

      if (window.hasFocus()) {
        Controls::updateKeys();
        window.clear();
        window.setView(view);
        // Main Loop code below here:

        sf::Vector2f mousePosition = (sf::Vector2f)window.mapPixelToCoords(
            sf::Mouse::getPosition(window));
        sf::Vector2f ts = midTiles.getTileSize();

        // Object step()'s here
        Entity::stepAll<Player>(Player::all, window, view, characterTexture,
                                frontTiles);

        drawBackground();
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

          if (inRange && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            shadows.setTile(mousePosition, 0);
            shadows.render();

            midTiles.removeTile(mousePosition);
            midTiles.render();

            frontTiles.removeTile(mousePosition);
            frontTiles.autoTile();
            frontTiles.render();
          }
          if ((inRange && !tooClose) &&
              sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
            shadows.setTile(mousePosition, 1);
            shadows.render();

            midTiles.addTile(mousePosition, wall);
            midTiles.render();

            frontTiles.addTile(mousePosition, wall);
            frontTiles.autoTile();
            frontTiles.render();
          }
        }

        // Draw foreground stuff
        drawForground();

        // Noise::draw(window);
        // Noise::sample(
        // (sf::Vector2f)window.mapPixelToCoords(sf::Mouse::getPosition(window))
        // / 300.f );

        guiView.setSize(view.getSize());
        window.setView(guiView);

        // gameOverText.setPosition( (sf::Vector2f)(window.getSize())/2.f );
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
      if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {

        if (keyPressed->scancode == sf::Keyboard::Scan::P) {
          return mainState;
        }
      }
    }
    if (window.hasFocus()) {
      window.clear();
      window.setView(view);
      mainState->drawBackground();
      mainState->drawForground();

      guiView.setSize(view.getSize());
      window.setView(guiView);
      window.draw(overlay);

      window.display();
    }
  }
  return nullptr;
}
#endif
