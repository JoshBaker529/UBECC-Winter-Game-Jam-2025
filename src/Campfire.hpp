#ifndef CAMPFIRE_HPP
#define CAMPFIRE_HPP

#include "DeltaTime.hpp"
#include "Entity.hpp"
#include "Tilemap.hpp"
#include "Particles.hpp"

class Entity;
class Tilemap;

class Campfire: public Entity{
private:
	sf::Clock clock;
	sf::Clock animationClock;
	sf::Clock smokeClock;
	bool lit = true;

public:

	Campfire(sf::Vector2f position): Entity(position, sf::Vector2f(32,32)) {}

	virtual void step(sf::RenderWindow &window, sf::View &view, sf::Texture &texture, Tilemap &tilemap){
		sf::Vector2f ts = tilemap.getTileSize();
		position = { floor(position.x/ts.x)*ts.x, floor(position.y/ts.y)*ts.y };
		recenterBoundingBox();
		
		const float dt = DeltaTime::get();

		int frame = animationClock.getElapsedTime().asSeconds() / 0.1f;
		frame %= 2;
		
		sf::Vector2f center = position + boundingBox.size/2.f;
		
		if(lit){
			if( !Particles::positionIsExposed(center,tilemap) ){
				clock.restart();
			}else{
				if(smokeClock.getElapsedTime().asSeconds() >= 0.05f){
					
					Particles::smoke( {center.x,center.y,1.f},sf::Color(64,64,64));
					smokeClock.restart();
				}
				
				if(clock.getElapsedTime().asSeconds() > 5.f){
					lit = false;
					clock.restart();
				}
			}
		}else{
			frame = 2;
			if(clock.getElapsedTime().asSeconds() > 5.f) dead = true;
		}

		// Draw Code
		sf::Sprite sprite(texture);
		sprite.setPosition( position );
		// sprite.setOrigin( sf::Vector2f(16.f,16.f) );
		sprite.setTextureRect( sf::IntRect( {frame*32,96}, {32,32}) );
		Entity::submitSprite(sprite);
		
	}
	
	static inline list<Campfire> all;
};

#endif