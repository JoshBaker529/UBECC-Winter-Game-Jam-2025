#ifndef PRODECURAL_HPP
#define PRODECURAL_HPP

#include "Utilities.hpp"
#include "Tilemap.hpp"
#include "Background.hpp"

#include <SFML/Graphics.hpp>

#include <vector>
using std::vector;
#include <list>
using std::list;
#include <unordered_map>
using std::unordered_map;
#include <unordered_set>
using std::unordered_set;

#include <cmath>
using std::fmod;

#include <iostream>
using std::cout;
using std::endl;
/*
	static inline sf::Vector2f height[DIVISIONS+1][DIVISIONS+1];
	
public:
	static void generate(){
		for(int y = 0; y < DIVISIONS+1; y++){
			for(int x = 0; x < DIVISIONS+1; x++){
				height[y][x].x = randFloat(-1.f,1.f);
				height[y][x].y = randFloat(-1.f,1.f);
				
				if( height[y][x] == sf::Vector2f(0.f,0.f) )
					height[y][x] = sf::Vector2f(1.f,0.f);
				
				height[y][x] = height[y][x].normalized();
			}
		}
	}
	*/
class Noise{
private:
	static inline const size_t DIVISIONS = 10;
	static inline float height[DIVISIONS+1][DIVISIONS+1];
	
public:
	static void generate(){
		for(int y = 0; y < DIVISIONS+1; y++){
			for(int x = 0; x < DIVISIONS+1; x++){
				height[y][x] = randFloat(-1.f,1.f);
			}
		}
	}
	
private:
	static float smoothStep(float low, float high, float amount){
		if(amount <= 0.f) return low;
		if(amount >= 1.f) return high;
		// return low + (amount*(high-low));
		
		
		
		float amountSquared = amount*amount;
		float amountCubed = amountSquared*amount;
		
		float raw = (3*amountSquared)-(2*amountCubed);
		
		return low+(raw*(high-low));
		// return raw*(high-low);
	}

	static float getHeight(sf::Vector2f position){
		// Expects floats between 0.f and 1.f (inclusive)
		if(position.x < 0.f) position.x = 0.f;
		if(position.y < 0.f) position.y = 0.f;
		if(position.x > 1.f) position.x = 1.f;
		if(position.y > 1.f) position.y = 1.f;
		
		const float cellSize = 1.f/DIVISIONS;
		sf::Vector2i index = (sf::Vector2i)(position/cellSize);
		
		return height[index.y][index.x];
	}
	
	static sf::Vector2i getIndex(sf::Vector2f position){
		// Expects floats between 0.f and 1.f (inclusive)
		if(position.x < 0.f) position.x = 0.f;
		if(position.y < 0.f) position.y = 0.f;
		if(position.x > 1.f) position.x = 1.f;
		if(position.y > 1.f) position.y = 1.f;
		
		const float cellSize = 1.f/DIVISIONS;
		sf::Vector2i index = (sf::Vector2i)(position/cellSize);
		
		// if(position.x < (cellSize*index.x)) cout << index.x;
		// if(position.y < (cellSize*index.y)) cout << index.x;
		
		
		return index;
	}

public:	
	static float sample(sf::Vector2f position){
		// Also expects floats between 0.f and 1.f (inclusive)
		if(position.x < 0.f || position.y < 0.f) return 0.f;
		if(position.x > 1.f || position.y > 1.f) return 0.f;
		
		const float cellSize = 1.f/DIVISIONS;
		
		// Height values (per corner)
		float tl, tr, br, bl;
		sf::Vector2i index = getIndex(position);
		tl = height[index.y][index.x];
		tr = height[index.y][index.x+1];
		br = height[index.y+1][index.x+1];
		bl = height[index.y+1][index.x];
		
		
		// tl = getHeight(position);
		// tr = getHeight(position + sf::Vector2f(cellSize,      0.f));
		// br = getHeight(position + sf::Vector2f(cellSize, cellSize));
		// bl = getHeight(position + sf::Vector2f(     0.f, cellSize));
		
		// Normalize relative position within the cell
		sf::Vector2f relative;
		relative.x = fmod(position.x,cellSize);
		relative.y = fmod(position.y,cellSize);
		relative *= (float)DIVISIONS;
		
		float topInterp    = smoothStep(tl, tr, relative.x);
		float bottomInterp = smoothStep(bl, br, relative.x);

		return smoothStep(topInterp,bottomInterp,relative.y);
		
	}
	
	static void draw(sf::RenderWindow &window){
		const float area = 300.f;
		
		sf::VertexArray verts;
		verts.setPrimitiveType(sf::PrimitiveType::Points);
		verts.clear();
		
		for(int y = 0; y < area; y++){
			for(int x = 0; x < area; x++){
				int c = sample( sf::Vector2f(x/300.f,y/300.f) )*128;
				c += 127;
				verts.append( sf::Vertex{ sf::Vector2f(x,y), sf::Color(c,c,c) } );
			}
		}
		window.draw(verts);
		
	}
};

class Procedural{
private:

	enum Terrain{ None, Grass, Dirt, Sand, Water, Stone, Snow, Count };
	struct Tile{
		list<Terrain> choices;
		Terrain chosenOne = Terrain::None;
	};
	
	// static void getTile(
	// sf::Vector2i position,
	// sf::Vector2i levelSize,
	// vector<vector<Tile>> &level){
		
		// if( position.x < 0 || position.y < 0) return;
		// if( position.x >= levelSize.x || position.y >= levelSize.y ) return;
		// rey
		
	// }
	
public:

	static void generateLevel(Background &background, Tilemap &tilemap){
		heightmap(background,tilemap);
	}
	
	static void heightmap(Background &background, Tilemap &tilemap){
		const sf::Vector2i levelSize = background.getSize();
		if(levelSize.x <= 0 || levelSize.y <= 0) return;
		
		vector<vector<float>> heightmap( levelSize.y, vector<float>(levelSize.x) );
		
		
	}

	static void waveFunctionCollapse(Background &background, Tilemap &tilemap){
		const sf::Vector2i levelSize = background.getSize();
		if(levelSize.x <= 0 || levelSize.y <= 0) return;
		
		vector<vector<Tile>> level( levelSize.y, vector<Tile>(levelSize.x) );

		// Populate choices (initally, anything is possible)
		for(int y = 0; y < levelSize.y; y++){
			for(int x = 0; x < levelSize.x; x++){
				
				for(int i = 1; i < Count; i++)
					level[y][x].choices.push_back( (Terrain)i );
				
			}
		}
		
		// Violations Map: If I'm Grass, what can my neighbors NOT be?
		unordered_map<Terrain, unordered_set<Terrain>> violations;
		violations[Grass] = unordered_set<Terrain>{ Water, Sand, Stone };
		violations[Dirt] =  unordered_set<Terrain>{ Water, Snow };
		violations[Sand] =  unordered_set<Terrain>{ Grass, Stone, Snow };
		violations[Water] = unordered_set<Terrain>{ Grass, Dirt,  Stone, Snow };
		violations[Stone] = unordered_set<Terrain>{ Grass, Water, Snow };
		violations[Snow] =  unordered_set<Terrain>{ Dirt,  Sand,  Water, Stone };
		
		size_t superpositions = levelSize.x * levelSize.y;
		
		level[0][0].chosenOne = Snow;
		superpositions--;
		
		// Eliminate choices that violate the rules
		for(int y = 1; y < levelSize.y-1; y++){
			for(int x = 1; x < levelSize.x-1; x++){
				
				// Top
				Terrain top = level[y-1][x].chosenOne;
				if( top != Terrain::None ){
					auto i = level[y][x].choices.begin();
					for(; i != level[y][x].choices.end(); i++){
						if( violations[top].find(*i) != violations[top].end() ){
							level[y][x].choices.erase(i);
							break;
						}
					}
				}
				
				if( level[y][x].choices.size() == 0 ){
					level[y][x].chosenOne = Terrain::None;
					goto fuck;
				}
								
				// Check the tile above me. Has it decided? If it has:
				// For any of my possibilites, is that chosen one in violation?
				// If so, remove it from my possibilites.
				// If I have none left, we're fucked!
			}
		}
		
		cout << superpositions << endl;
		
		// Now the choices are all valid. Randomly select one.
		for(int y = 0; y < levelSize.y; y++){
			for(int x = 0; x < levelSize.x; x++){
				int randomIndex = rand() % level[y][x].choices.size();
				auto theOne = level[y][x].choices.begin();
				
				for(int i = 0; i < randomIndex; i++) theOne++;
				level[y][x].chosenOne = *theOne;
			}
		}
		
		fuck:
		
		// Output
		for(int y = 0; y < levelSize.y; y++){
			for(int x = 0; x < levelSize.x; x++){
				int texture = level[y][x].chosenOne;
				background.setTileAtIndex( sf::Vector2i(x,y), texture );
			}
		}
		
		background.render();
		tilemap.render();
	}

	// static void generateLevel(Background &background){
		// const int LEVEL_WIDTH  = background.getSize().x;
		// const int LEVEL_HEIGHT = background.getSize().y;
		
		// unordered_map<int, vector<int> > map;
		
		// map[0] = vector<int>{ 1 };
		// map[1] = vector<int>{ 1, 1, 0, 2, 6 };
		// map[2] = vector<int>{ 2, 2, 3, 6 };
		// map[3] = vector<int>{ 3, 3, 1, 2, 6 };
		// map[6] = vector<int>{ 6, 1, 1 };
		
		// int tile;
		
		// for(int y = 0; y < LEVEL_HEIGHT; y++){
			// for(int x = 0; x < LEVEL_WIDTH; x++){
				
				// int top  = background.getTileAtIndex( sf::Vector2i(x,y-1) );
				// int left = background.getTileAtIndex( sf::Vector2i(x-1,y) );
				
				// vector<int> *leftOutcomes = &map[left];
				// vector<int> *topOutcomes  = &map[top];
				
				// vector<int> finalOutcomes;
				// for(int i = 0; i < topOutcomes->size(); i++){
					// for(int j = 0; j < leftOutcomes->size(); j++){
						// if( (*topOutcomes)[i] == (*leftOutcomes)[j] ) 
							// finalOutcomes.push_back( (*topOutcomes)[i] );
					// }
				// }
				
				// if( finalOutcomes.size() == 0 )
					// tile = 1;
				// else
					// tile = finalOutcomes[ rand() % finalOutcomes.size() ];
				
				// background.setTileAtIndex( sf::Vector2i(x,y), tile );
				
			// }
		// }
	// }
};

#endif