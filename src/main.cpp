#define COMPILE_MAIN_CPP
#ifdef COMPILE_MAIN_CPP

#include <SFML/Graphics.hpp>

#include "Controls.hpp"
#include "Tilemap.hpp"
#include "Entity.hpp"
#include "Player.hpp"


#include <iostream>
using std::cout;
using std::endl;



int main()
{

    sf::RenderWindow window(sf::VideoMode({ 1024, 768 }), "");
    window.setVerticalSyncEnabled(true);

    sf::View view;
    view.setSize((sf::Vector2f)window.getSize());
    view.setCenter(sf::Vector2f(0, 0));    

	sf::Texture frontTileset;
	if(!frontTileset.loadFromFile("res/frontTiles.png")) return 1;
	if(!frontTileset.generateMipmap()) return 1;

	Controls::addButton(sf::Keyboard::Key::W);
	Controls::addButton(sf::Keyboard::Key::A);
	Controls::addButton(sf::Keyboard::Key::S);
	Controls::addButton(sf::Keyboard::Key::D);

	Tilemap tilemap;
	Tilemap::Tile wall;
	wall.type = Tilemap::Tile::Type::solid;
	wall.vanity = sf::Vector2i(0,0);
	
	tilemap.addTile( sf::Vector2f(0,0), wall);
	tilemap.render();
	
	Player player;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) window.close();
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                view.setSize((sf::Vector2f)resized->size);
            }
            else if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {
                view.zoom(mouseWheelScrolled->delta == 1 ? 0.9f : (1/0.9f));
            }
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
				if (keyPressed->scancode == sf::Keyboard::Scan::R){
					if(!frontTileset.loadFromFile("res/frontTiles.png")) return 1;
					if(!frontTileset.generateMipmap()) return 1;
				}
			}
        }
		
		if(window.hasFocus()){
			Controls::updateKeys();
			window.clear();
			window.setView(view);
			// Main Loop code below here:

			sf::Vector2f mousePosition = (sf::Vector2f)window.mapPixelToCoords(sf::Mouse::getPosition(window));
			if( sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) ){
				tilemap.addTile(mousePosition,wall);
				tilemap.autoTile();
				tilemap.render();
			}
			if( sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) ){
				tilemap.removeTile(mousePosition);
				tilemap.autoTile();
				tilemap.render();
			}
			tilemap.draw(window,frontTileset);
			
			player.step(window,view,tilemap);

			window.setView(view);
		}
        window.display();
    }
}

#endif