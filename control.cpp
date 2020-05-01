#include "snake.h"
#include "ui.h"
#include "control.h"

Human::Human(Game& g) : Control(g) {
    using namespace std::placeholders;
    Ui::get()->on_key(std::bind(&Human::keypressed, this, _1));
}

void Human::keypressed(int key) {
    switch (key) {
        case ' ':
            if (game.paused) {
                game.paused = 0;
                Ui::get()->painter("        S n a k e __ G a m e        ");
            } else {
                game.paused = 1;
                Ui::get()->painter("G a m e __ P a u s e d (Press SPACE)", true);
            }
            break;
    }

    if (stopped || game.paused)
        return;

    switch(key)
    {
        case Ui::KEY_UP:
            //if (dir != Snake::DOWN)
                temp_dir = Snake::UP;
                //set_direction(Snake::UP);
            break;
        case Ui::KEY_DOWN:
            //if (dir != Snake::UP)
                temp_dir = Snake::DOWN;
                //set_direction(Snake::DOWN);
            break;
        case Ui::KEY_RIGHT:
            //if (dir != Snake::LEFT)
                temp_dir = Snake::RIGHT;
                //set_direction(Snake::RIGHT);
            break;
        case Ui::KEY_LEFT:
            //if (dir != Snake::RIGHT)
                temp_dir = Snake::LEFT;
                //set_direction(Snake::LEFT);
            break;
    }
}

Robot::Robot(Game& g) : Control(g) {
    Ui::get()->on_timer(Game::TICK, std::bind(&Robot::tick, this));
}

//BETA
void Robot::tick() {
    if (stopped)
        return;

    Ui::get()->on_timer(Game::TICK, std::bind(&Robot::tick, this));

    Direction d[] = { UP, DOWN, LEFT, RIGHT };
    std::random_shuffle(d, d + 4);

    auto s = body.front();
    auto r = game.rabbit_near(s);

    if (r.has_value()) {
        for (int i = 0; i < 4; ++i) {
            set_direction(d[i]);
            Coord n = next();
            if (game.is_clear(n, true) && r->distance(n) < r->distance(s))
                return;
        }
    }

    for (int i = 0; i < 4; ++i) {
        set_direction(d[i]);
        Coord n = next();
        if (game.is_clear(n, true))
            return;
    }
}
