#ifndef SWORD_HPP
#define SWORD_HPP

#include "Entity.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
// #include "Animal.hpp"
#include <list>
using std::list;

class Sword: public Entity{
private:
	sf::Angle swing = sf::degrees(-90);
	bool lethal = true;
	
public:

	Sword() : Entity(position, sf::Vector2f(48, 48)){
		
	}
	
	virtual void step(sf::RenderWindow &window, sf::View &view, sf::Texture &texture, Tilemap &tilemap){
		if( Player::all.empty() ){
			dead = true;
			return;
		}
		
		if( swing.asDegrees() > 90) dead = true;
		
		Player *player = &Player::all.front();		
		
		sf::Vector2f cursor = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		sf::Angle angle = (cursor-player->getPosition()).angle();
		
		swing += sf::degrees(10.f);
		angle += swing;
		position = player->getPosition() + ( sf::Vector2f(32.f,0.f).rotatedBy(angle) );
		recenterBoundingBox();
		
		if(lethal){
			for(auto i = regularSnowman::all.begin(); i != regularSnowman::all.end(); i++){
				if( !collision(*i) ) continue;
				sf::Vector2f to = i->getPosition()-position;
				if(to != sf::Vector2f(0.f,0.f) ){ 
					i->applyForce(to.normalized()*10.f);
					Particles::burst( {i->getPosition().x,i->getPosition().y,0.f}, sf::Color::White );
					i->health -= 20.f;
					if(i->health <= 0.f) i->kill();
					lethal = false;
				}
			}
			
			for(auto i = ghostSnowman::all.begin(); i != ghostSnowman::all.end(); i++){
				if( !collision(*i) ) continue;
				sf::Vector2f to = i->getPosition()-position;
				if(to != sf::Vector2f(0.f,0.f) ){ 
					i->applyForce(to.normalized()*10.f);
					Particles::burst( {i->getPosition().x,i->getPosition().y,0.f}, sf::Color::White );
					i->health -= 5.f;
					if(i->health <= 0.f) i->kill();
					lethal = false;
				}
			}
		}
		
		// for(auto i = Animal:all.begin(); i != Animal::all.end(); i++){
			// if( !collision(*i) ) continue;
			// sf::Vector2f to = i->getPosition()-position;
			// if(to != sf::Vector2f(0.f,0.f) ){ 
				// i->applyForce(to.normalized()*10.f);
				// Particles::burst( {i->getPosition().x,i->getPosition().y,0.f}, sf::Color::White );
			// }
		// }
		
		sf::Sprite sprite(texture);
		sprite.setPosition(position);
		sprite.setOrigin(sf::Vector2f(0.f, 32.f));
		sprite.setTextureRect(sf::IntRect({192, 0}, {32, 32}));
		sprite.setRotation(angle+sf::degrees(45));
		Entity::submitSprite(sprite);
	}
	
	static inline list<Sword> all;
};

#endif