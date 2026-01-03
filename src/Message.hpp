#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <SFML/Graphics.hpp>

#include <iostream>
using namespace::std;

class Message{
public:
	enum Priority{ Trivial, Important, Critical };
	
private:

	static inline string currentMessage = "";
	static inline Priority currentPriority = Trivial;
	static inline sf::Clock clock;

public:
	
	static void send(string message, Priority priority = Trivial){
		if( (int)priority < (int)currentPriority ) return;
		currentMessage = message;
		currentPriority = priority;
		clock.restart();
	}
	
	static void draw(sf::RenderWindow &window, sf::View &view, sf::Font &font){
		if(clock.getElapsedTime().asSeconds() >= 5.f){
			currentPriority = Trivial;
			return;
		}
		
		sf::Text text(font);
		text.setCharacterSize(18);
		text.setFillColor(sf::Color::White);
		text.setStyle(sf::Text::Bold);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(2);
		text.setString(currentMessage);
		
		float textWid = text.getGlobalBounds().size.x;
		text.setPosition( {-(textWid/2.f), 0.f} );
		
		window.draw(text);
	}
	
};

#endif