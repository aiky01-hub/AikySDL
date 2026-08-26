#pragma once
#include "timer.h"

class Animation {
    Timer timer;
    int frameCount;

   public:
    Animation() : frameCount(0), timer(0) {}

    Animation(int frameCount, float length) : frameCount(frameCount), timer(length) {};

    float getLength() const { return timer.getLength(); }
    int currentFrame() const {
        return static_cast<int>(timer.getTime() / timer.getLength() * frameCount);
    }

    void step(float deltaTime) {
        timer.step(deltaTime);
    }
};