#pragma once
#include "timer.h"

class Animation {
    Timer timer;
    int frameCount;

   public:
    Animation() : timer(0), frameCount(0) {}

    Animation(int frameCount, float length) : frameCount(frameCount), timer(length) {};

    float getLength() const { return timer.getLength(); }
};