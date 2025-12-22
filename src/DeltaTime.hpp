#ifndef DELTATIME_HPP
#define DELTATIME_HPP

class DeltaTime {
private:
    static inline float deltaTime = 1.f;
public:
    static float get() { return deltaTime; }
    static float set(float newDeltaTime) { deltaTime = newDeltaTime; }
};

#endif