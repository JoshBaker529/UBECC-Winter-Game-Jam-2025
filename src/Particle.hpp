#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "Utilities.hpp"

#include <list>
using std::list;


class Explosions{
private:

	class Particle{
	public:
		sf::Vector2f position, move;
		static inline const size_t LIFESPAN = 60;
		float counter = 0, variation;
		
		Particle(sf::Vector2f position, sf::Vector2f move ){
			this->position = position;
			this->move = move;
			variation = randFloat(-15,15);
		}
	};
	
	static inline list<Particle> particles;
	static inline sf::VertexArray verts;
	
	static sf::Color getParticleColor(size_t time){
		const size_t numberOfColors = 5;
		static const sf::Color colors[numberOfColors] = {
			sf::Color(255,174,0), sf::Color(255,94,0), sf::Color(255,200,0), sf::Color(99,99,99), sf::Color(99,99,99) 
		};
		size_t index = std::min( numberOfColors-1, (size_t)( (time/((float)Particle::LIFESPAN))*numberOfColors) );
		return colors[index];
	}
	
public:
	static void makeParticle(sf::Vector2f position, sf::Vector2f move){
		particles.push_back( Particle(position, move));
	}
	
	static void makeBurst(sf::Vector2f position, float spread, float speed, int amount = 20){
		for(int i = 0; i < amount; i++){
			sf::Angle randomAngle = sf::degrees( randFloat(0,360) );
			float randomForce = randFloat(0,1);
			sf::Vector2f randomOffset( randFloat(-spread,spread), randFloat(-spread,spread) );
			makeParticle(position, sf::Vector2f(1,0).rotatedBy(randomAngle)*randomForce*speed );
		}
	}
	
	static void draw(sf::RenderWindow &window){
		if(particles.empty()) return;
		
		verts.setPrimitiveType(sf::PrimitiveType::Triangles);
		verts.clear();
		
		// Simulate, Submit for drawing
		sf::Color particleColor;
		float time;
		for(auto i = particles.begin(); i != particles.end(); i++){
			time = i->counter; // Add delta time calculation
			if(time >= Particle::LIFESPAN) particles.erase(i);
			
			i->position += i->move;
			
			particleColor = getParticleColor(i->counter);
			particleColor.a = (1.f-(time/(float)Particle::LIFESPAN) )*255;
			verts.append( sf::Vertex{ i->position, particleColor } );
			verts.append( sf::Vertex{ i->position+sf::Vector2f(i->variation,0.f), particleColor } );
			verts.append( sf::Vertex{ i->position+sf::Vector2f(0.f,i->variation), particleColor } );
			
			i->counter++;
		}
		
		window.draw(verts);
	}		
	
};

#endif