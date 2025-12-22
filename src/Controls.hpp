#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <SFML/Graphics.hpp>

#include<unordered_map>
using std::unordered_map;


class Controls {
private:
    struct entry {
        int keyValue = -1;
        bool tapped = false, held = false, released = false;
    };
    static inline unordered_map<sf::Keyboard::Key,entry> keys;

    static inline unordered_map<sf::Mouse::Button,entry> mouseButtons;
    static inline sf::RenderWindow* window = NULL;
    static inline sf::Vector2i mousePos, mousePosLast;

    Controls() {};
public:
    static void addButton(sf::Keyboard::Key keyToAdd) {
        if (keys.find(keyToAdd) != keys.end()) return;
        entry newKey; newKey.keyValue = (int)keyToAdd;
        keys[keyToAdd] = newKey;
    }
    static void updateKeys() {
        for (auto i = keys.begin(); i != keys.end(); i++) {
            
            i->second.tapped = false;
            i->second.released = false;

            if (sf::Keyboard::isKeyPressed( (sf::Keyboard::Key)i->second.keyValue )) {
                if (!i->second.held) i->second.tapped = true;
                i->second.held = true;
            }
            else {
                if (i->second.held) i->second.released = true;
                i->second.held = false;
            }
        }
    }

    static void addButton(sf::Mouse::Button buttonToAdd) {
        if (mouseButtons.find(buttonToAdd) != mouseButtons.end()) return;
        entry newGuy; newGuy.keyValue = (int)buttonToAdd;
        mouseButtons[buttonToAdd] = newGuy;
    }
    static void windowRelativeTo(sf::RenderWindow& window) { Controls::window = &window; }
    static void updateMouse() {
        for (auto i = mouseButtons.begin(); i != mouseButtons.end(); i++) {
            
            i->second.tapped = false;
            i->second.released = false;

            if (sf::Mouse::isButtonPressed( (sf::Mouse::Button)i->second.keyValue )) {
                if (!i->second.held) i->second.tapped = true;
                i->second.held = true;
            }
            else {
                if (i->second.held) i->second.released = true;
                i->second.held = false;
            }
        }

        mousePosLast = mousePos;
        if (window) mousePos = sf::Mouse::getPosition(*window);
        else mousePos = sf::Mouse::getPosition();
    }
	
	

    static bool held(sf::Keyboard::Key keyToLookFor) {
        if (keys.find(keyToLookFor) == keys.end()) return false;
        return keys[keyToLookFor].held;
    }
    static bool tapped(sf::Keyboard::Key keyToLookFor) {
        if (keys.find(keyToLookFor) == keys.end()) return false;
        return keys[keyToLookFor].tapped;
    }
    static bool released(sf::Keyboard::Key keyToLookFor) {
        if (keys.find(keyToLookFor) == keys.end()) return false;
        return keys[keyToLookFor].released;
    }

    static bool held(sf::Mouse::Button buttonToLookFor) {
        if (mouseButtons.find(buttonToLookFor) == mouseButtons.end()) return false;
        return mouseButtons[buttonToLookFor].held;
    }
    static bool tapped(sf::Mouse::Button buttonToLookFor) {
        if (mouseButtons.find(buttonToLookFor) == mouseButtons.end()) return false;
        return mouseButtons[buttonToLookFor].tapped;
    }
    static bool released(sf::Mouse::Button buttonToLookFor) {
        if (mouseButtons.find(buttonToLookFor) == mouseButtons.end()) return false;
        return mouseButtons[buttonToLookFor].released;
    }

    static sf::Vector2i mousePosition() { return mousePos; }
    static sf::Vector2i LastMousePosition() { return mousePosLast; }
    static sf::Vector2i mouseDelta() { return mousePos-mousePosLast; }
};

#endif