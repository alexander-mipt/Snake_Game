#include <memory>
#include <cstdio>

#include "ui.h"
#include "control.h"

int main() {

  std::unique_ptr<Ui> v(Ui::get("sfm"));
  if (!v) {
    fprintf(stderr, "Cannot init UI\n");
    return 0;
  }

  Game g;
  Human h(g);
  Robot s1(g), s2(g);//, s3(g);

  v->run(g);
}
