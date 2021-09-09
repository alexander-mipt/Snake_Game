#include <memory>
#include <cstdio>
#include <cassert>
#include <cstring>

#include "ui.h"
#include "control.h"

int main(int argc, char** argv) {
  assert(argc <= 2);
  const char* interface = "tty";
  if (argc == 2) {
    interface = argv[1];
    assert(!std::strcmp(interface, "tty") || !std::strcmp(interface, "sfm"));
  }
  // tty or sfm
  std::unique_ptr<Ui> v(Ui::get(interface));
  if (!v) {
    fprintf(stderr, "Cannot init UI\n");
    return 0;
  }

  Game g;
  Human h(g);
  Robot s1(g), s2(g);//, s3(g);

  v->run(g);
}
