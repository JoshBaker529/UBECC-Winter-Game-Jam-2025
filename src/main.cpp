#define COMPILE_MAIN_CPP
#ifdef COMPILE_MAIN_CPP

#include <SFML/Graphics.hpp>

#include "Controls.hpp"
#include "DeltaTime.hpp"
#include "Background.hpp"
#include "Tilemap.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "Decor.hpp"
#include "Procedural.hpp"
#include "Particles.hpp"

#include <iostream>
using std::cout;
using std::endl;

int main(){

    sf::RenderWindow window(sf::VideoMode({ 1024, 768 }), "");
    window.setVerticalSyncEnabled(true);

    sf::View view;
    view.setSize((sf::Vector2f)window.getSize());
    view.setCenter(sf::Vector2f(0, 0));
	
	sf::View guiView;
    guiView.setCenter(sf::Vector2f(0, 0));

	sf::Clock clock;

	// Textures
	sf::Texture frontTileset;
	if(!frontTileset.loadFromFile("res/frontTiles.png")) return 1;
	if(!frontTileset.generateMipmap()) return 1;
	
	sf::Texture backTileset;
	if(!backTileset.loadFromFile("res/backTiles.png")) return 1;
	if(!backTileset.generateMipmap()) return 1;
	
	sf::Texture characterTexture;
	if(!characterTexture.loadFromFile("res/characters.png")) return 1;
	if(!characterTexture.generateMipmap()) return 1;
	
	sf::Texture shadowTexture;
	if(!shadowTexture.loadFromFile("res/shadow.png")) return 1;
	if(!shadowTexture.generateMipmap()) return 1;

	sf::Font font;
	if(!font.openFromFile("res/tuffy.ttf")) return 1;

	// Tilemaps
	Background background( sf::Vector2i(100,100), sf::Vector2f(32.f, 32.f), sf::Vector2f(96.f, 96.f), sf::Vector2f(-32.f,-32.f) );
	
	Background shadows( sf::Vector2i(100,100), sf::Vector2f(32.f, 32.f), sf::Vector2f(96.f, 96.f), sf::Vector2f(-32.f,-32.f) );
	
	Tilemap midTiles( sf::Vector2f(32.f,32.f), sf::Vector2f(32.f,32.f) );
	midTiles.render();
	
	Tilemap frontTiles( sf::Vector2f(32.f,32.f), sf::Vector2f(32.f,32.f) );
	frontTiles.setOffset( sf::Vector2f(0.f,-32.f) );
	frontTiles.render();
	
	Tilemap::Tile wall;
	wall.type = Tilemap::Tile::Type::solid;
	wall.vanity = sf::Vector2i(3,2);

	// Controls
	Controls::addButton(sf::Keyboard::Key::W);
	Controls::addButton(sf::Keyboard::Key::A);
	Controls::addButton(sf::Keyboard::Key::S);
	Controls::addButton(sf::Keyboard::Key::D);
	
	Noise::generate();
	Procedural::generateLevel(background,frontTiles);
	Player::all.push_back( Player() );
	Decor::all.push_back( Decor( {100.f,50.f}, {24.f,24.f}, {32,80}, sf::IntRect{ {32,0}, {64,96} } ));

	// Extra crap
	sf::Text gameOverText(font);
	gameOverText.setFillColor(sf::Color::White);
	gameOverText.setOutlineColor(sf::Color::Black);
	gameOverText.setString("GAME OVER - Press R to Respawn.");
	gameOverText.setCharacterSize(30);
	gameOverText.setStyle(sf::Text::Bold);
	gameOverText.setOrigin( gameOverText.getLocalBounds().size/2.f);

    while (window.isOpen())
    {
		DeltaTime::set( clock.getElapsedTime().asSeconds() / (1.f/60.f) );
		if( DeltaTime::get() > 2.f ) DeltaTime::set(2.f);
		clock.restart();
		
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
				else if (keyPressed->scancode == sf::Keyboard::Scan::G){
					Procedural::generateLevel(background,frontTiles);
					background.render();
				}
				
			}
        }
		
		if(window.hasFocus()){
			Controls::updateKeys();
			window.clear();
			window.setView(view);
			// Main Loop code below here:

			sf::Vector2f mousePosition = (sf::Vector2f)window.mapPixelToCoords(sf::Mouse::getPosition(window));
			sf::Vector2f ts = midTiles.getTileSize();
			
			// Object step()'s here
			Entity::stepAll<Player>(Player::all,window,view,characterTexture,frontTiles);
			Entity::stepAll<Decor>(Decor::all,window,view,characterTexture,frontTiles);			
			
			// Draw background stuff
			background.draw(window,backTileset);
			shadows.draw(window,shadowTexture);
			midTiles.draw(window,frontTileset);
			
			// Object draw()'s here
			Entity::drawAll(window);
			Particles::draw(window);
			
			// If the player is alive, do block stuffs
			if(Player::all.size() > 0){
				Player *player = &Player::all.front();
				bool inRange = abs(mousePosition.x-player->getPosition().x) <= 128.f;
				inRange = inRange && abs(mousePosition.y-player->getPosition().y) <= 128.f;
				
				sf::Vector2f fp( floor(mousePosition.x/ts.x), floor(mousePosition.y/ts.y));
				fp *= ts.x;
				
				sf::RectangleShape blockCursor;
				blockCursor.setPosition( fp );
				blockCursor.setFillColor(sf::Color::Transparent);
				blockCursor.setOutlineThickness(1);
				blockCursor.setOutlineColor(sf::Color(255,255,255,128));
				blockCursor.setSize(ts);
				
				sf::FloatRect blockRect{ fp, ts };
				bool tooClose = blockRect.findIntersection(player->getBoundingBox()) != std::nullopt;
				if(tooClose) blockCursor.setOutlineColor(sf::Color(255,0,0,128));
				
				if( inRange ){
					window.draw(blockCursor);
					blockCursor.move( {0.f,-ts.y} );
					window.draw(blockCursor);
				}
				
				if( inRange && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) ){
					shadows.setTile( mousePosition, 0 );
					shadows.render();
					
					midTiles.removeTile(mousePosition);
					midTiles.render();
					
					frontTiles.removeTile(mousePosition);
					frontTiles.autoTile();
					frontTiles.render();
				}
				if( (inRange && !tooClose) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) ){
					shadows.setTile( mousePosition, 1 );
					shadows.render();
					
					midTiles.addTile(mousePosition,wall);
					midTiles.render();
					
					frontTiles.addTile(mousePosition,wall);
					frontTiles.autoTile();
					frontTiles.render();
				}
			}
			
			// Draw foreground stuff
			frontTiles.draw(window,frontTileset);
			
			// Noise::draw(window);
			// Noise::sample( (sf::Vector2f)window.mapPixelToCoords(sf::Mouse::getPosition(window)) / 300.f );

			guiView.setSize(view.getSize());
			window.setView(guiView);
			
			// gameOverText.setPosition( (sf::Vector2f)(window.getSize())/2.f );
			if(Player::all.size() <= 0) window.draw(gameOverText);
		}		
		
        window.display();
		
    }
}

#endif