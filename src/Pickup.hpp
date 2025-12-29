#ifndef PICKUP_HPP
#define PICKUP_HPP

#include <SFML/Graphics.hpp>

class Pickup: public Entity{
private:
	size_t itemId = 0;
	
public:

	Pickup(sf::Vector2f position, sf::Vector2f size, sf::Vector2f origin, sf::IntRect textureRect): Entity(position, size) {
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
	
	static inline list<Pickup> all;
};

#endif