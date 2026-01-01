#ifndef PARTICLES_HPP
#define PARTICLES_HPP

#include "Utilities.hpp"
#include "DeltaTime.hpp"

#include <list>
using std::list;

/*

Different kinds of paticles:
	-Block Break
	-Blood
	-Wind
	
Factora:
	-Gravity
	-Weight (weightless particles could float?

*/

class Particles{
private:
	
	struct Particle{
		sf::Vector3f position, move;
		sf::Vector2f variation;
		sf::Color color;
		float weight, lifespan;
	};

	static inline list<Particle> particles;
	static inline sf::VertexArray verts;
	static inline sf::Vector3f wind;
	
public:

	static void make(
	sf::Vector3f position, sf::Vector3f move, sf::Color color, float weight){
		Particle particle{ position, move, {0.f,0.f}, color, weight, 120.f };
		
		particle.move.x += randFloat(-1.f,1.f);
		particle.move.y += randFloat(-1.f,1.f);
		
		particle.variation.x += randFloat(-10.f,10.f);
		particle.variation.y += randFloat(-10.f,10.f);
		
		particles.push_back(particle);
	}
	
	static void makeRange(
	sf::Vector3f position, sf::Vector3f range,
	sf::Vector3f move, sf::Color color, float weight){
		
		position.x += randFloat(0.f, range.x);
		position.y += randFloat(0.f, range.y);
		position.z += randFloat(0.f, range.z);
		
		make(position, move, color, weight);
		
	}
	
	static sf::Vector2f project(sf::Vector3f position){
		return sf::Vector2f( position.x, position.y-position.z );
	}
	
	static void draw(sf::RenderWindow &window){
		if(particles.empty()) return;
		
		verts.setPrimitiveType(sf::PrimitiveType::Triangles);
		verts.clear();
		
		sf::Vector3f gravity(0.f,0.f,-0.1f);
		wind = sf::Vector3f(0.1f,0.f,0.f);
		
		// Simulate, Submit for drawing
		for(auto i = particles.begin(); i != particles.end(); i++){
			if(i->lifespan < 0) particles.erase(i);
			
			if(i->position.z <= 0.f){
				i->position.z = 0.f;
				i->move = {0.f,0.f,0.f};
			}else{
				i->move += gravity*i->weight;
				i->move += wind;
				i->position += i->move;
			}
			
			sf::Vector2f v1,v2,v3;
			v1 = project(i->position);
			v2 = v1 + i->variation;
			v3 = v1 + i->variation.perpendicular();
			
			verts.append( sf::Vertex{ v1, i->color} );
			verts.append( sf::Vertex{ v2, i->color} );
			verts.append( sf::Vertex{ v3, i->color} );
			
			i->lifespan -= DeltaTime::get();
		}
		
		window.draw(verts);
	}

};

/*
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
*/

#endif