/* Control Manager */
#pragma once

#include "snake.h"

// Snake control
class Control : public Snake {
public:
    Control(Game& g) : Snake(g), game(g) { g.add(*this); }
protected:
    Game& game;
};

// Gamer Snake control
class Human : public Control {
public:
    Human(Game& g);
    void keypressed(int key);
};

// Robot Snake control
class Robot : public Control {
public:
    Robot(Game& g);
    void tick();
};
