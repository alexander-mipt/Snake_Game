#include <cstring>

#include "ui.h"
#include "tui.h"
#include "sfm.h"

Ui* Ui::instance;

Ui* Ui::get(const char* what) {
  if (!instance) {
    if (strcmp(what, "tty") == 0)
      instance = new Tty();
    if (strcmp(what, "sfm") == 0)
      instance = new Window();
  }
  //printf("Ui* p = %p\n", instance);
  return instance;
}

Ui::~Ui() {
  instance = nullptr;
}
