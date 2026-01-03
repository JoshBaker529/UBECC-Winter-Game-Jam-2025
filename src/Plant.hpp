#ifndef PLANT_HPP
#define PLANT_HPP

#include "Tilemap.hpp"
#include "Decor.hpp"

class Plant: public Decor{
public:

	enum Type{ Tree, BerryBush, DeadBush, Carrot, Grass };
	Type type;

	Plant(sf::Vector2f position, Type type): Decor(position) {
		this->type = type;
		
		switch(type){
			case Tree:
				textureRect = { {0,0}, {64,96} };
				origin = {32.f,80.f};
				break;
			case BerryBush:
				textureRect = { {64,0}, {32,32} };
				origin = {16.f,16.f};
				break;
			case DeadBush:
				textureRect = { {96,0}, {32,32} };
				origin = {16.f,16.f};
				break;
			case Grass:
				textureRect = { {128,0}, {32,32} };
				origin = {16.f,16.f};
				break;
			case Carrot:
				textureRect = { {160,0}, {32,32} };
				origin = {16.f,16.f};
				break;
		}
			
	}

	virtual void step(sf::RenderWindow &window, sf::View &view, sf::Texture &texture, Tilemap &tilemap){		
		Decor::step(window,view,texture,tilemap);
	}
	
	static inline list<Plant> all;
};

#endif