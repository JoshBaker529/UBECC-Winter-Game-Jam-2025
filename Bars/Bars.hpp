#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


#define HEALTHVALUE  
#define HUNGERVALUE
#define WARMTHVALUE 

#define BARWIDTH  100.f;
#define BARHEIGHT  25.f;

class Resource_Bars 
{

public:
    
    

    Resource_Bars(int healthinit, int hungerinit, int warmthinit);
private:

    float healthval;
    float hungerval;
    float warmthval;

    sf::RectangleShape healthbar ({BARWIDTH, BARHEIGHT});

    sf::RectangleShape hungerbar ({BARWIDTH, BARHEIGHT});

    sf::RectangleShape warmthbar ({BARWIDTH, BARHEIGHT});


}