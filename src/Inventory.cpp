
#include "Inventory.hpp"
#include "SFML/Graphics/Texture.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

Inventory::Inventory(sf::Vector2f vec) {
  start = vec;

  for (int i = 0; i < SLOTS; i++) {
  }

  // NOTE:
  // Explanation of vertexCount:
  // SLOTS  -- number of inventory slots
  // * 2    -- One for the base layer, one for the items layer
  // + 1    -- Background rectangle
  array = sf::VertexArray(sf::PrimitiveType::Triangles,
                          ((SLOTS * 2) + 1) * VERTICES_PER_SQUARE);

  array[0].position = sf::Vector2f{-350.f, -300.f};
  array[1].position = sf::Vector2f{350.f, -300.f};
  array[2].position = sf::Vector2f{350.f, 0.f};
  array[3].position = sf::Vector2f{-350.f, -300.f};
  array[4].position = sf::Vector2f{-350.f, 0.f};
  array[5].position = sf::Vector2f{350.f, 0.f};

  array[0].color = sf::Color(BACKGROUND_COLOR);
  array[1].color = sf::Color(BACKGROUND_COLOR);
  array[2].color = sf::Color(BACKGROUND_COLOR);
  array[3].color = sf::Color(BACKGROUND_COLOR);
  array[4].color = sf::Color(BACKGROUND_COLOR);
  array[5].color = sf::Color(BACKGROUND_COLOR);

  float x = start.x;
  float y = start.y;

  float slot_width = WIDTH / COLUMNS;
  float slot_height = HEIGHT / ROWS;

  float border_width = slot_width * .05;
  float boarder_height = slot_height * .05;

  // Base Layer
  for (int slot = 0; slot < 21; slot++) {
    if (slot % COLUMNS == 0 && slot != 0) {
      x = start.x;
      y = y + slot_height;
    }

    int index = slot * 6 + 6;
    sf::Vector2f UpLeft(x + border_width, y + boarder_height),
        UpRight(x + slot_width - border_width, y + boarder_height),
        DownLeft(x + border_width, y + slot_height - boarder_height),
        DownRight(x + slot_width - border_width,
                  y + slot_width - boarder_height);

    array[index + 0].position = UpLeft;
    array[index + 1].position = UpRight;
    array[index + 2].position = DownRight;
    array[index + 3].position = DownRight;
    array[index + 4].position = DownLeft;
    array[index + 5].position = UpLeft;
    array[index + 0].color = sf::Color(SLOT_COLOR);
    array[index + 1].color = sf::Color(SLOT_COLOR);
    array[index + 2].color = sf::Color(SLOT_COLOR);
    array[index + 3].color = sf::Color(SLOT_COLOR);
    array[index + 4].color = sf::Color(SLOT_COLOR);
    array[index + 5].color = sf::Color(SLOT_COLOR);
    x += slot_width;
  }

  x = start.x;
  y = start.y;
  // Item Layer
  for (int slot = 0; slot < 21; slot++) {
    if (slot % COLUMNS == 0 && slot != 0) {
      x = start.x;
      y = y + slot_height;
    }

    int index_start = (SLOTS + 1) * 6;
    int index = slot * 6 + index_start;
    sf::Vector2f UpLeft(x + border_width, y + boarder_height),
        UpRight(x + slot_width - border_width, y + boarder_height),
        DownLeft(x + border_width, y + slot_height - boarder_height),
        DownRight(x + slot_width - border_width,
                  y + slot_width - boarder_height);

    array[index + 0].position = UpLeft;
    array[index + 1].position = UpRight;
    array[index + 2].position = DownRight;
    array[index + 3].position = DownRight;
    array[index + 4].position = DownLeft;
    array[index + 5].position = UpLeft;

    if (slot_filled[slot]) {
      sf::Vector2f item_texture = inventory[slot].getTexturePosition();
      sf::Vector2f TextureUpLeft(item_texture),
          TextureUpRight(item_texture.x + TEXTURE_WIDTH, item_texture.y),
          TextureDownLeft(item_texture.x, item_texture.y + TEXTURE_HEIGHT),
          TextureDownRight(item_texture.x + TEXTURE_WIDTH,
                           item_texture.y + TEXTURE_HEIGHT);

      array[index + 0].texCoords = TextureUpLeft;
      array[index + 1].texCoords = TextureUpRight;
      array[index + 2].texCoords = TextureDownRight;
      array[index + 3].texCoords = TextureDownRight;
      array[index + 4].texCoords = TextureDownLeft;
      array[index + 5].texCoords = TextureUpLeft;
      x += slot_width;
    }
  }
}

Inventory::~Inventory() {
  // idk, I'm not using pointers ..
}

bool Inventory::isOpen() { return open; }

void Inventory::toggleOpen() { open ^= 1; }

sf::VertexArray Inventory::getArray() { return array; }

void Inventory::draw(sf::RenderWindow window) {

  Item item("Greg", sf::Vector2f{100, 100}, 1, CONSUMABLE);

  inventory[1] = item;
  slot_filled[1] = true;

  window.draw(array);
}
