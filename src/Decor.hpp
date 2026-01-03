#ifndef DECOR_HPP
#define DECOR_HPP

#include "Entity.hpp"
#include <SFML/Graphics.hpp>

class Decor: public Entity{
protected:
	sf::IntRect textureRect;
	sf::Vector2f origin;
	
public:

	Decor(sf::Vector2f position): Entity(position, sf::Vector2f(32.f,32.f)) {
		this->position = position;
	}

	Decor(sf::Vector2f position, sf::Vector2f size, sf::Vector2f origin, sf::IntRect textureRect): Entity(position, size) {
		this->position = position;
		this->textureRect = textureRect;
		this->origin = origin;
	}

	virtual void step(sf::RenderWindow &window, sf::View &view, sf::Texture &texture, Tilemap &tilemap){
		sf::Sprite sprite(texture);
		sprite.setPosition( getBoundingBox().position + (getBoundingBox().size/2.f) );
		sprite.setOrigin( origin );
		sprite.setTextureRect( textureRect );
		Entity::submitSprite(sprite);
		
	}
	
	static inline list<Decor> all;
};

#endif
