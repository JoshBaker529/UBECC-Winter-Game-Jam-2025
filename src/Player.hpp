#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Controls.hpp"
#include "DeltaTime.hpp"
#include "Entity.hpp"
#include "Tilemap.hpp"
#include "Particles.hpp"

#include<list>
using std::list;

class Entity;
class Tilemap;

class Player: public Entity{
public:

	Player(): Entity(sf::Vector2f(0,0), sf::Vector2f(24,24)) {}

	virtual void step(sf::RenderWindow &window, sf::View &view, sf::Texture &texture, Tilemap &tilemap){
		const float dt = DeltaTime::get();
		const float momentum = 0.5f*dt, friction = 0.25f*dt, terminal = 4.f;
		
		// Input
		sf::Vector2f input;
		input.x = (float)Controls::held(sf::Keyboard::Key::D)-(float)Controls::held(sf::Keyboard::Key::A);
		input.y = (float)Controls::held(sf::Keyboard::Key::S)-(float)Controls::held(sf::Keyboard::Key::W);
		if(input != sf::Vector2f{0,0} ) input = input.normalized();
		
		// sf::Vector2f vs = view.getSize();
		// Particles::makeRange( {position.x-(vs.x/2.f),position.y-(vs.y/2.f),100.f}, {0.f,vs.y,vs.y}, {0.f,0.f,0.f}, sf::Color::White, 0.1f);
		
		// Movement
		applyForce(input*momentum);
		applyResistance(friction);
		cap(terminal);
		move(tilemap);
		
		// Move Camera
		view.move( (position-view.getCenter())/1.f );
		window.setView(view);
		
		// Draw Code
		sf::Sprite sprite(texture);
		sprite.setPosition( getBoundingBox().position + (getBoundingBox().size/2.f) );
		sprite.setOrigin( sf::Vector2f(16.f,48.f) );
		sprite.setTextureRect( sf::IntRect( {0,0}, {32,64}) );
		Entity::submitSprite(sprite);
		
	}
	
	static inline list<Player> all;
};

#endif