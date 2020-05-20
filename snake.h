/* Game Controller */
#pragma once
#include <cstdio>

#include <list>
#include <utility>
#include <optional>
#include <functional>

// Coordinates (x0,y0) & method distance (from self to (x,y))
struct Coord : std::pair<int,int> {
    using Base = std::pair<int,int>;
    using Base::Base;
    int distance(const Coord& c) const {
        return std::abs(first - c.first) + std::abs(second - c.second);
    }
};

class Game;

// Snake class
class Snake {
public:
    enum Direction {
        UP,
        DOWN,
        RIGHT,
        LEFT,
        BODY,
        NONE,
    };

    // deleting default constructors for correct work of new snakes
    explicit Snake(Game&);
    Snake(const Snake&) = delete;
    Snake& operator=(const Snake&) = delete;

    void set_direction(enum Direction d) { dir = d; }

    Coord next(); // get coordinates of next step for snake
    void move(std::list<Coord>& rabbits); // SNAKE MOVEMENT

    bool stopped; // snake movement flag
    Direction dir;
    int brand; // Snake id
    std::list<Coord> body; // coordinates of body

protected:
    Game& game;
    Direction temp_dir;

private:
    const int start_len = 3;
};

// Snake Segement (Coordinates + snake_id + move dir)
struct Segment : Coord {
    Segment(const Coord& c, int b, Snake::Direction d)
        : Coord(c), brand(b), dir(d) {}
    int brand;
    Snake::Direction dir;
};

// Rabbit (struct Coord)
struct Rabbit : Coord, std::optional<int> {
    Rabbit(const Coord& c, const std::optional<int>& d = std::nullopt)
        : Coord(c), std::optional<int>(d) {} // DELETE std::optional
    
    
};

/* Game manager */
class Game {
public:
    enum {
        TICK = 80, // milliseconds game tick
        RABBITS_CHANCE = 20, // 10% chance per game tick
        MAX_RABBITS = 10, // max rabbits on screen
    };

    Game();

    void add(Snake& s); // add Snake into snakes' list and only
    void add(Coord c = {0,0}); // add rabbit

    void move(); // snake steps -> CALL SNAKE PAINTER
    void draw(); // first draw - add snakes on game field -> CALL SNAKE PAINTER

    Coord randxy();

    bool is_clear(Coord, bool skip_rabbits=false); // bump test
    int paused; // game pause / play
    std::optional<Coord> rabbit_near(Coord); // return Coord of the nearest rabbit

private:
    std::list<std::reference_wrapper<Snake>> snakes; // list of snakes
    std::list<Coord> rabbits; // list of rabbits
};
