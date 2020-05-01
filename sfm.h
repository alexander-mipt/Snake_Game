#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

#include <time.h> /////////////////
#include <signal.h>
#include <poll.h>

#include <cstdio>

#include "ui.h"

class Elem : public sf::Sprite {
public:
	sf::Image image;
	sf::Texture texture;
	Elem(const char name);
	Elem() = default;
	~Elem();

	const int pixel_size;
};



class Window : public Ui {
public:

	Window();
	~Window();
	
	/* Realization of virtual Ui methods */

	int winx() const override { return density.width / pixel_size - 1; }
  	int winy() const override { return density.height / pixel_size - 1; }

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
	
	const int pixel_size;
	void place_pixel(int x, int y, int angle, sf::Sprite& pixel);
	
	sf::RenderWindow win;
private:

    bool running;
	sf::VideoMode density;
	
	
	
	void set_screen_size();
    bool read_event();


    std::vector<event_fn> events; // vector of key requests
    std::vector<std::pair<int, timer_fn>> timers; // vector of active timers
    sf::Sprite field[500][500];


};