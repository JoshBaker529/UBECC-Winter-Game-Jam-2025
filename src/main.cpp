#define COMPILE_MAIN_CPP
#ifdef COMPILE_MAIN_CPP

#include "StateManager.hpp"

int main(){
	
    StateManager manager;
	manager.start( StateManager::getMainState() );
    
	return 0;
}

#endif