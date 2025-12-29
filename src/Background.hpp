#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <SFML/Graphics.hpp>

#include<vector>
using std::vector;

class Background{
private:
	sf::VertexArray verts;
	vector<vector<int>> tiles;
	
	sf::Vector2i size;
	sf::Vector2f tileSize;
	sf::Vector2f textureSize;
	sf::Vector2f offset;
	
public:
	Background(sf::Vector2i size, sf::Vector2f tileSize, sf::Vector2f textureSize, sf::Vector2f offset){
		this->size = size;
		tiles.resize(size.y);
		for(int i = 0; i < size.y; i++) tiles[i].resize(size.x);
		
		this->tileSize = tileSize;
		this->textureSize = textureSize;
		this->offset = offset;
	}
	
	sf::Vector2i getSize(){ return size; }
	
	void clear(){
		for(int y = 0; y < size.y; y++){
			for(int x = 0; x < size.x; x++){
				tiles[y][x] = 0;
			}
		}
	}
	
	sf::Vector2i worldToGridCoords(sf::Vector2f in) {
		// in -= offset;
		int x = (int)(in.x / tileSize.x);
		int y = (int)(in.y / tileSize.y);
		return {x - (in.x < 0), y - (in.y < 0)};
	}
	
	int getTileAtIndex(sf::Vector2i index){
		if(index.x < 0 || index.y < 0 || index.x >= size.x || index.y >= size.y) return 0;
		return tiles[index.y][index.x];		
	}
	
	void setTileAtIndex(sf::Vector2i index, int value){
		if(index.x < 0 || index.y < 0 || index.x >= size.x || index.y >= size.y) return;
		tiles[index.y][index.x] = value;
	}
	
	int getTile(sf::Vector2f position) {
		return getTileAtIndex(worldToGridCoords(position));
	}
	
	void setTile(sf::Vector2f position, int value) {
		setTileAtIndex(worldToGridCoords(position), value);
	}
	
	void renderTile(int x, int y){
		int texture = tiles[y][x];		
		if( texture <= 0 ) return;
		
		sf::Vector2f v[4];
		sf::Color c;
		sf::Vector2f t[4];
		
		// Vertex Positions
		v[0] = sf::Vector2f( x*tileSize.x, y*tileSize.y );
		v[1] = v[0] + sf::Vector2f( textureSize.x, 0 );
		v[2] = v[0] + sf::Vector2f( textureSize.x, textureSize.y );
		v[3] = v[0] + sf::Vector2f( 0, textureSize.y );
		
		// Apply offset
		for(int i = 0; i < 4; i++){
			v[i] += offset;
		}		
		// Vertex Color
		c = sf::Color::White;
		
		// Texture Coordinates
		t[0] = sf::Vector2f( texture*textureSize.x, 0 );
		t[1] = t[0] + sf::Vector2f( textureSize.x, 0 );
		t[2] = t[0] + sf::Vector2f( textureSize.x, textureSize.y );
		t[3] = t[0] + sf::Vector2f( 0, textureSize.y );
		
		// Done
		verts.append(sf::Vertex{v[0], c, t[0]});
		verts.append(sf::Vertex{v[1], c, t[1]});
		verts.append(sf::Vertex{v[2], c, t[2]});

		verts.append(sf::Vertex{v[0], c, t[0]});
		verts.append(sf::Vertex{v[2], c, t[2]});
		verts.append(sf::Vertex{v[3], c, t[3]});
	}
	
	void render(){		
		verts.clear();
		verts.setPrimitiveType(sf::PrimitiveType::Triangles);
		
		for(int y = 0; y < size.y; y += 1){
			if( y % 2 == 0 )
				for(int x = 0; x < size.x; x += 1) renderTile(x,y);
			else
				for(int x = size.x-1; x >= 0; x -= 1) renderTile(x,y);
		}
		
	}
	
	void draw(sf::RenderWindow &window, sf::Texture &texture){
		window.draw(verts,&texture);
	}
	
};

#endif