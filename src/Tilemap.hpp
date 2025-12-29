#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include <SFML/Graphics.hpp>

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
using std::size_t;
using std::string;
using std::to_string;
using std::unordered_map;

#include <cmath>
using std::ceil;
using std::min;

#include <utility>
using std::swap;

#include <iostream>
using std::cout;
using std::endl;

class Tilemap {
public:
	struct Tile {
		enum Type { air, solid, grate, pitfall, count };

		sf::Vector2i vanity;
		Type type = Type::air;
		uint8_t rotate = 0;
		bool flipX = false;
		bool flipY = false;
	};

private:
	struct Vector2iHash {
		size_t operator()(const sf::Vector2i &in) const {
			size_t x = std::hash<int>()(in.x), y = std::hash<int>()(in.y);
			return y ^ (x << 1);
		}
	};

	unordered_map<sf::Vector2i, Tile, Vector2iHash> tiles;
	sf::VertexArray verts;
	sf::Vector2f tileSize = sf::Vector2f{32.f, 32.f};
	sf::Vector2f textureSize = sf::Vector2f{32.f, 32.f};
	sf::Vector2f offset = sf::Vector2f(0.f, 0.f);

public:
	Tilemap() {}

	Tilemap(sf::Vector2f tileSize) {
		this->tileSize = tileSize;
		this->textureSize = tileSize;
	}
	Tilemap(sf::Vector2f tileSize, sf::Vector2f textureSize) {
		this->tileSize = tileSize;
		this->textureSize = textureSize;
	}

	const sf::Vector2f getTileSize() { return tileSize; }
	const sf::Vector2f getTextureSize() { return textureSize; }
	
	sf::Vector2f getOffset(){ return offset; }
	void setOffset(sf::Vector2f offset){ this->offset = offset; }

	sf::Vector2i worldToGridCoords(sf::Vector2f in) {
		// in -= offset;
		int x = (int)(in.x / tileSize.x);
		int y = (int)(in.y / tileSize.y);
		return {x - (in.x < 0), y - (in.y < 0)};
	}

	const Tile *getTileAtIndex(sf::Vector2i position) {
		static const Tile defaultTile{sf::Vector2i(0, 0), Tile::Type::air, 0, false, false};
		if (tiles.find(position) == tiles.end()) return &defaultTile;
		return &tiles[position];
	}
	void addTileAtIndex(sf::Vector2i position, Tile &tileToAdd) {
		if (tiles.find(position) != tiles.end()) return;
		tiles[position] = tileToAdd;
	}
	void removeTileAtIndex(sf::Vector2i position) { tiles.erase(position); }

	const Tile *getTile(sf::Vector2f position) {
		return getTileAtIndex(worldToGridCoords(position));
	}
	void addTile(sf::Vector2f position, Tile &tileToAdd) {
		addTileAtIndex(worldToGridCoords(position), tileToAdd);
	}
	void removeTile(sf::Vector2f position) {
		removeTileAtIndex(worldToGridCoords(position));
	}

	void clear(){
		verts.clear();
		tiles.clear();
	}

	bool collision(sf::Vector2f position) {
		const Tile *found = getTile(position);
		return found->type != Tile::Type::air;
	}

	bool collisionRect(sf::FloatRect rect) {
		if (rect.size.x <= tileSize.x && rect.size.y <= tileSize.y) {
			if (collision(sf::Vector2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y))) return true;
			if (collision(sf::Vector2f(rect.position.x, rect.position.y + rect.size.y))) return true;
			if (collision(sf::Vector2f(rect.position.x + rect.size.x, rect.position.y))) return true;
			if (collision(sf::Vector2f(rect.position.x, rect.position.y))) return true;
			return false;
		}

		sf::Vector2f i;

		float x1 = rect.position.x;
		float y1 = rect.position.y;
		float x2 = rect.position.x + rect.size.x;
		float y2 = rect.position.y + rect.size.y;

		for (i.y = y1; i.y < (y2 + tileSize.y); i.y += tileSize.y) {
			for (i.x = x1; i.x < (x2 + tileSize.x); i.x += tileSize.x) {
				if (collision(sf::Vector2f(min(i.x, x2), min(i.y, y2))))
				  return true;
			}
		}

		return false;
	}

	void autoTile() { // Do in a range?
		for (auto i = tiles.begin(); i != tiles.end(); i++) {
			sf::Vector2i position = i->first;
			Tile *tile = &(i->second);
			
			uint8_t bits = 0;
			if (getTileAtIndex(position + sf::Vector2i( 1,  0))->type == Tile::Type::solid) bits += 1; // Right
			if (getTileAtIndex(position + sf::Vector2i( 1,  1))->type == Tile::Type::solid) bits += 2; // Bottom Right
			if (getTileAtIndex(position + sf::Vector2i( 0,  1))->type == Tile::Type::solid) bits += 4; // Bottom
			if (getTileAtIndex(position + sf::Vector2i(-1,  1))->type == Tile::Type::solid) bits += 8; // Bottom Left
			if (getTileAtIndex(position + sf::Vector2i(-1,  0))->type == Tile::Type::solid) bits += 16; // Left
			if (getTileAtIndex(position + sf::Vector2i(-1, -1))->type == Tile::Type::solid) bits += 32; // Top Left
			if (getTileAtIndex(position + sf::Vector2i( 0, -1))->type == Tile::Type::solid) bits += 64; // Top
			if (getTileAtIndex(position + sf::Vector2i( 1, -1))->type == Tile::Type::solid)	bits += 128; // Top Right
			
			*tile = getTileOrientation(bits);
		}
	}

	void render() {
		verts.clear();
		verts.setPrimitiveType(sf::PrimitiveType::Triangles);

		sf::Vector2f v[4];
		sf::Color c[4];
		sf::Vector2f t[4];

		const float small = 0.01f;

		for (auto i = tiles.begin(); i != tiles.end(); i++) {
			// Vertex Positions
			v[0] = sf::Vector2f{i->first.x*tileSize.x, i->first.y*tileSize.y};
			v[1] = v[0] + sf::Vector2f{tileSize.x, 0};
			v[2] = v[0] + sf::Vector2f{tileSize.x, tileSize.y};
			v[3] = v[0] + sf::Vector2f{0, tileSize.y};
			
			// Apply offset
			for(int i = 0; i < 4; i++){
				v[i] += offset;
			}

			// Vertex Colors
			c[0] = c[1] = c[2] = c[3] = sf::Color::White;

			// Texture Coordinates
			t[0] = ((sf::Vector2f)i->second.vanity) * textureSize.x;
			t[1] = t[0] + sf::Vector2f{textureSize.x, 0};
			t[2] = t[0] + sf::Vector2f{textureSize.x, textureSize.y};
			t[3] = t[0] + sf::Vector2f{0, textureSize.y};

			// Evil rendering hack to reduce tearing
			t[0] += sf::Vector2f(small, small);
			t[1] += sf::Vector2f(-small, small);
			t[2] += sf::Vector2f(-small, -small);
			t[3] += sf::Vector2f(small, -small);

			// Flip X and Y
			if (i->second.flipX) {
				swap(t[0].x, t[1].x);
				swap(t[2].x, t[3].x);
			}
			if (i->second.flipY) {
				swap(t[0].y, t[3].y);
				swap(t[1].y, t[2].y);
			}

			// Rotation
			uint8_t numberOfRotations = i->second.rotate % 4;
			for (uint8_t r = 0; r < numberOfRotations; r++) {
				sf::Vector2f temp = t[0];
				t[0] = t[3];
				t[3] = t[2];
				t[2] = t[1];
				t[1] = temp;
			}

			// Done
			verts.append(sf::Vertex{v[0], c[0], t[0]});
			verts.append(sf::Vertex{v[1], c[1], t[1]});
			verts.append(sf::Vertex{v[2], c[2], t[2]});

			verts.append(sf::Vertex{v[0], c[0], t[0]});
			verts.append(sf::Vertex{v[2], c[2], t[2]});
			verts.append(sf::Vertex{v[3], c[3], t[3]});
		}
	}

	void draw(sf::RenderWindow &window) { 
		window.draw(verts); 
	}
	void draw(sf::RenderWindow &window, sf::Texture &texture) {
		window.draw(verts, &texture);
	}
	void draw(sf::RenderWindow &window, sf::Shader &shader) {
		window.draw(verts, &shader);
	}

	string toString() {
		string out = "";
		for (auto i = tiles.begin(); i != tiles.end(); i++) {
			out += to_string(i->first.x) + ' ';
			out += to_string(i->first.y) + ' ';
			out += to_string(i->second.vanity.x) + ' ';
			out += to_string(i->second.vanity.y) + ' ';
			out += to_string((size_t)i->second.type) + ' ';
			out += to_string(i->second.rotate) + ' ';
			out += to_string(i->second.flipX) + ' ';
			out += to_string(i->second.flipY) + '\n';
		}
		return out;
	}

	private:
	// uint8_t rotateL(uint8_t i, size_t amount) {
		// amount %= 8;
		// return (i << amount | i >> (8 - amount));
	// }

	uint8_t rotateR(uint8_t i, size_t amount) {
		amount %= 8;
		return (i >> amount | i << (8 - amount));
	}

	Tile getTileOrientation(uint8_t binary) {
		const static unordered_map<uint8_t, std::pair<sf::Vector2i, uint8_t>>
		mapping{
			{0b0000'0000, {{0, 0}, 0}},

			{0b0000'0001, {{1, 0}, 0}}, {0b0000'0100, {{1, 0}, 1}},
			{0b0001'0000, {{1, 0}, 2}}, {0b0100'0000, {{1, 0}, 3}},

			{0b0000'0101, {{2, 0}, 0}}, {0b0001'0100, {{2, 0}, 1}},
			{0b0101'0000, {{2, 0}, 2}}, {0b0100'0001, {{2, 0}, 3}},

			{0b0001'0001, {{3, 0}, 0}}, {0b0100'0100, {{3, 0}, 1}},

			{0b0001'0101, {{4, 0}, 0}}, {0b0101'0100, {{4, 0}, 1}},
			{0b0101'0001, {{4, 0}, 2}}, {0b0100'0101, {{4, 0}, 3}},

			{0b0101'0101, {{5, 0}, 0}},

			{0b1111'1111, {{0, 1}, 0}},

			{0b0111'1111, {{1, 1}, 0}}, {0b1111'1101, {{1, 1}, 1}},
			{0b1111'0111, {{1, 1}, 2}}, {0b1101'1111, {{1, 1}, 3}},

			{0b0101'1111, {{2, 1}, 0}}, {0b0111'1101, {{2, 1}, 1}},
			{0b1111'0101, {{2, 1}, 2}}, {0b1101'0111, {{2, 1}, 3}},

			{0b0111'0111, {{3, 1}, 0}}, {0b1101'1101, {{3, 1}, 1}},

			{0b0101'0111, {{4, 1}, 0}}, {0b0101'1101, {{4, 1}, 1}},
			{0b0111'0101, {{4, 1}, 2}}, {0b1101'0101, {{4, 1}, 3}},

			{0b0000'0111, {{5, 1}, 0}}, {0b0001'1100, {{5, 1}, 1}},
			{0b0111'0000, {{5, 1}, 2}}, {0b1100'0001, {{5, 1}, 3}},

			{0b0001'0111, {{0, 2}, 0}}, {0b0101'1100, {{0, 2}, 1}},
			{0b0111'0001, {{0, 2}, 2}}, {0b1100'0101, {{0, 2}, 3}},

			{0b0001'1101, {{1, 2}, 0}}, {0b0111'0100, {{1, 2}, 1}},
			{0b1101'0001, {{1, 2}, 2}}, {0b0100'0111, {{1, 2}, 3}},

			{0b0001'1111, {{2, 2}, 0}}, {0b0111'1100, {{2, 2}, 1}},
			{0b1111'0001, {{2, 2}, 2}}, {0b1100'0111, {{2, 2}, 3}},
		};

		getRidOfCorners(binary);
		if (mapping.count(binary) < 1) {
			std::cout << "BAD NUMBER\n";
			std::cout << std::hex << (size_t)binary << '\n';
			std::exit(0);
		}
		
		auto [pos, rotate] = mapping.at(binary);
		return Tile{pos, Tile::solid, rotate};
	}

	void getRidOfCorners(uint8_t &binary) {
		uint8_t cornerMask = 0b00000101;
		uint8_t theBit = 0b11111101;
		for (size_t i = 0; i < 4; ++i) {
			if ((binary & cornerMask) != cornerMask) {
				binary = binary & theBit;
			}
			binary = rotateR(binary, 2);
		}
	}
};

#endif
