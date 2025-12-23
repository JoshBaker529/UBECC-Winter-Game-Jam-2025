#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Controls.hpp"
#include "DeltaTime.hpp"
#include "Entity.hpp"
#include "Tilemap.hpp"

class Entity;
class Tilemap;

class Player: public Entity{
public:

	Player(): Entity(sf::Vector2f(0,0), sf::Vector2f(31,31)) {}

	virtual void step(sf::RenderWindow &window, sf::View &view, Tilemap &tilemap){
		const float dt = DeltaTime::get();
		const float momentum = 0.5f*dt, friction = 0.25f*dt, terminal = 6.f;
		
		// Input
		sf::Vector2f input;
		input.x = (float)Controls::held(sf::Keyboard::Key::D)-(float)Controls::held(sf::Keyboard::Key::A);
		input.y = (float)Controls::held(sf::Keyboard::Key::S)-(float)Controls::held(sf::Keyboard::Key::W);
		if(input != sf::Vector2f{0,0} ) input = input.normalized();
		
		// Movement
		applyForce(input*momentum);
		applyResistance(friction);
		cap(terminal);
		move(tilemap);
		
		// Move Camera
		view.move( (position-view.getCenter())/1.f );
		window.setView(view);
		
		// Draw
		sf::RectangleShape rect;
		rect.setPosition( getBoundingBox().position );
		rect.setSize( getBoundingBox().size );
		rect.setFillColor( sf::Color::Red );
		window.draw(rect);
	}
};

#endif