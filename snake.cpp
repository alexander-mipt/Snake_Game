#include <climits>
#include <random>

#include "ui.h"
#include "snake.h"

static std::random_device rseed;
static std::mt19937 rgen(rseed());

Snake::Snake(Game& g) : stopped(false), dir(RIGHT), game(g), temp_dir(NONE) {
    static int b;
    
    Coord c;

    brand = ++b;

    
    do {
        c = g.randxy();
    } while (c.first < start_len + 2);
    
    body.push_back(c);
    for (int i = 0; i < start_len; ++i) {
        c.first--;
        body.push_back(c);
    }

}

Coord Snake::next() {
    auto c = body.front();
    switch(dir) {
        case UP:
            --c.second; // bug fixed
            break;
        case DOWN:
            ++c.second; // bug fixed
            break;
        case RIGHT:
            ++c.first;
            break;
        case LEFT:
            --c.first;
            break;
        default:
            break;
    }
    return c;
}

std::optional<Coord> Game::rabbit_near(Coord a) {
    std::optional<Coord> near;
    int distance = INT_MAX;

    for (auto const& r : rabbits) {
        int d = r.distance(a);
        if (d < distance) {
            distance = d;
            near = r;
        }
    }

    return near;
} 

Game::Game() : paused(1) {
    Ui::get()->on_timer(TICK, std::bind(&Game::move, this));
}


Coord Game::randxy() {
    std::uniform_int_distribution<int> xdist(2, Ui::get()->winx()-3);
    std::uniform_int_distribution<int> ydist(2, Ui::get()->winy()-3);

    Coord c;
    do {
        c = Coord(xdist(rgen), ydist(rgen));
    } while (!is_clear(c));

    return c;

}

void Game::add(Snake& s) {
    snakes.push_back(s);
}

void Snake::move(std::list<Coord>& rabbits) {
    if (stopped)
        return;

    if (    temp_dir != NONE && 
            !(temp_dir == DOWN && dir == UP) && 
            !(temp_dir == UP && dir == DOWN) &&
            !(temp_dir == RIGHT && dir == LEFT) &&
            !(temp_dir == LEFT && dir == RIGHT) )
        
        dir = temp_dir;

    auto a = body.back();
    auto b = body.front();
    auto c = next();

    auto v = Ui::get();
    v->painter(brand, body.size() - 1); // score

    auto i = std::find(rabbits.begin(), rabbits.end(), c);
    if (i == rabbits.end() && game.is_clear(c)) { // bug fixed
        body.pop_back();
    } else if (i != rabbits.end()) {
        rabbits.erase(i);
        //v->painter(brand, body.size() - 1);
    }

    if (!game.is_clear(c)) {
        stopped = true;
        return;
    }

    body.push_front(c);

    v->painter({c, brand, dir});
    if (a != b)
        v->painter({b, brand, BODY});
    v->painter({a, brand, NONE});

}

//MAIN FUNCTION ---------------------------
void Game::move() {
    static int ticks = 0;
    Ui::get()->on_timer(TICK, std::bind(&Game::move, this));
    if (paused == 1 && ticks > 0) return;
    

    for (Snake &s : snakes) {
        s.move(rabbits);
    }

    Game::add();
    ticks++;
}

void Game::add(Coord c) {
    std::uniform_int_distribution<int> dice(1,100/Game::RABBITS_CHANCE);
    if (rabbits.size() < Game::MAX_RABBITS && dice(rgen) == 1) {
        Coord place;
        if (c.first != 0 && c.second != 0)
            place = c;
        else
            place = randxy();
        
        rabbits.push_back(place);
        Ui::get()->painter(place);
    }

    
}

void Game::draw() {
    auto v = Ui::get();

        for (const Snake& s : snakes) {
                auto d = s.dir;
                for (const auto& c : s.body) {
                        v->painter({c, s.brand, d});
                        d = Snake::BODY;
                }
                v->painter(s.brand, s.body.size());
        }

        for (const Rabbit& r : rabbits)
                v->painter(r);
}

bool Game::is_clear(Coord c0, bool skip_rabbits) {
    auto v = Ui::get();
    auto x = v->winx();
    auto y = v->winy();
    
    if (c0.first >= x || c0.second >= y || c0.first <= 1 || c0.second <= 1)
        return false;

    for (const Snake& s : snakes) {
        for (const Coord& c : s.body) {
            if (c0.first == c.first && c0.second == c.second)
                return false;
        }
    }

    return true;
}
