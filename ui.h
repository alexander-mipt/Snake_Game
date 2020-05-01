/* VIRTUAL USER INTERFACE CLASS */
#pragma once

#include "snake.h"

class Ui {
public:
  using event_fn = std::function<void(int)>; // key client (subscribe on key)
  using timer_fn = std::function<void(void)>; // timer client (subscribe on timer)

  enum { // special keys only
    KEY_UP = 1000,
    KEY_DOWN,
    KEY_RIGHT,
    KEY_LEFT,
  };

  // interface pointer (game methods use it to get control resourses)
  static Ui* get(const char* item = NULL); // if call get() then item = NULL

  // return width & height of openned window
  virtual int winx() const = 0;
  virtual int winy() const = 0;

  // frame manager (updates the picture, timers) - see. cpp for more info
  virtual void run(Game&) = 0;
  
  virtual void draw() = 0; // draw picture (borders & score)
  virtual void quit() = 0; // Cntrl+C handler

  virtual ~Ui() = 0;

  virtual void on_key(event_fn fn) = 0;
  virtual void on_timer(int, timer_fn fn) = 0;

  // snake painter
  virtual void painter(const Segment&) = 0;
  // rabbit painter
  virtual void painter(const Rabbit&) = 0;
  // score painter
  virtual void painter(int, int) = 0;
  // text in game
  virtual void painter(const char* str, bool highight = false) = 0;

protected:
  Ui() = default;

private:
  static Ui* instance; // pointer for containing user interface class
};
