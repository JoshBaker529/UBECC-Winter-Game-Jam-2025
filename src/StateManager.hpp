#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP
#include "State.hpp"

class StateManager {
private:
  inline static MainState *mainState = new MainState();
  inline static State *currentState = nullptr;

public:
  static void start(State *startState) {

    currentState = startState;
    while (true) {
      if (currentState == nullptr) {
        break;
      }
      State *temp = currentState->loop(mainState);
      if (currentState != mainState) {
        delete currentState;
      }

      currentState = temp;
    }
  }

  static MainState *getMainState() { return mainState; }
};

#endif
