/* Text user interface (realization) - VIEW CONTROLLER */
#pragma once

#include <termios.h>
#include <sys/ioctl.h>
#include <vector>

#include <signal.h>
#include <unistd.h>

#include "ui.h"

class Tty : public Ui {
public:
  Tty();
  ~Tty();

  /* Realization of virtual Ui methods */

  int winx() const override { return ws.ws_col; }
  int winy() const override { return ws.ws_row; }

  void run(Game&) override;
  void quit() override;

  void on_key(event_fn fn) override;
  void on_timer(int t, timer_fn fn) override;

  void painter(const Segment&) override;
  void painter(const Rabbit&) override;
  void painter(int, int) override;
  void painter(const char* str, bool highlight = false) override;

  void draw() override;

   /* END of virtual methods */


private:
  void cls(); // clear screen

  void putc(int x, int y, char c); // draw symbol c in (x,y) from (1,1) to (winx, winy)

  void hline(int x, int y, int len); // draw horizontal line
  void vline(int x, int y, int len); // draw vertical line

  void setcolor(int color); // set color of drawing pen
  void gotoxy(int x, int y); // place drawing pen to (x,y)
  bool read_event(); // read key to create game request

  void init_tty(int echo); // init print settings for correct display (see more in .cpp) 
  void fini_tty(); // return default terminal settings
  void winch(); // window size init (??)

private:
  struct winsize ws; // window info
  struct termios ttys; // terminal info
  bool running; // frame changing flag
  Game* game; // not used here

  std::vector<event_fn> events; // vector of key requests
  std::vector<std::pair<int, timer_fn>> timers; // vector of active timers
};
