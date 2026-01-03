#ifndef PICKUP_HPP
#define PICKUP_HPP

#include "DeltaTime.hpp"
#include "ItemList.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "Message.hpp"

#include <SFML/Graphics.hpp>

class Pickup: public Entity{
private:
	string type;
	float z = 1;
	float zMove = 4.f;
	
public:

	Pickup(sf::Vector2f position, string type): Entity(position, sf::Vector2f(32.f,32.f)) {
		this->type = type;
	}

	virtual void step(sf::RenderWindow &window, sf::View &view, sf::Texture &texture, Tilemap &tilemap){
		Item item = ITEM(type,1);
		
		if(Player::all.size() > 0){
			Player *player = &Player::all.front();
			if( collision(*player) ){
				dead = true;
				
				Player::inventory.addItem(item);
				Message::send("Picked up " + type + ".");
				return;
			}
		}
		
		float dt = DeltaTime::get();
		
		if(z <= 0){
			z = 0;
		}else{
			zMove -= 0.2f*dt;
			z += zMove*dt;
		}
		
		
		// sf::RectangleShape r;
		// r.setPosition(boundingBox.position);
		// r.setSize(boundingBox.size);
		// r.setFillColor(sf::Color::Red);
		// window.draw(r);
		
		// Draw
		sf::Sprite sprite(texture);
		sprite.setPosition(position );
		sprite.setOrigin( {16.f,16.f} );
		sprite.move( sf::Vector2f(0.f,-z) );
		
		int tx = item.getTexturePosition().x;
		sprite.setTextureRect( sf::IntRect{ {tx,0}, {32,32} } );
		
		
		Entity::submitSprite(sprite);
		
	}
	
	static inline list<Pickup> all;
};

#endif