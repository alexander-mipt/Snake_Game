#include "sfm.h"

#include <signal.h>
#include <unistd.h>

#define P_SIZE 32

Elem::Elem(const char name) : pixel_size(P_SIZE) {
    //printfprintf("I'm born %p\n", this);
	switch (name) {
		case 'w':
			image.loadFromFile("./Wall32_32.png");
			break;
		case 'r':
			image.loadFromFile("./Rabbit32_32.png");
			break;
		case 's':
			image.loadFromFile("./Snake32_32.png");
			break;
	}
	
	texture.loadFromImage(image);
	this->setTexture(texture);
    //sf::IntRect size = this->getTextureRect();
    //printf("size = %d\n", size.width);
    //int scale = pixel_size / size.width; 
    //this->setScale(scale,scale);
}

Elem::~Elem() {
    //printf("I'm dyed %p\n", this);
}

Window::Window() : pixel_size(P_SIZE) {

	set_screen_size();
	win.create(density, "Lesson");
    sf::Sprite sp;
	
	/*for (int i = 0; i < 500; i++)
        for (int j = 0; j < 500; j++) {
            printf("here init\n");
            field[i][j] = sp;
        }*/

}

void Window::set_screen_size() {
    sf::VideoMode fullscreen = sf::VideoMode::getDesktopMode();
    int width = fullscreen.width;
    int height = fullscreen.height;
	width -= width % pixel_size;
	height -= height % pixel_size;
	//printf("w = %d, h = %d\n", width, height);
    density.width = width;
	density.height = height;
}



void Window::place_pixel(int x, int y, int angle, sf::Sprite& pixel) {
	pixel.setOrigin(pixel_size / 2, pixel_size / 2);
	pixel.setRotation(90. * angle);
	pixel.setPosition( (x + 0.5) * pixel_size , (y + 0.5) * pixel_size);
	//win.draw(pixel);
    field[x][y] = pixel;
	
}

void Window::draw() {
    Elem brick('w');
    
    for (int i = 0; i <= winx(); i++) {
        this->place_pixel(i, 0, 0, brick);
        this->place_pixel(i, winy(), 0, brick);
    }

    for (int i = 1; i <= winy() - 1; i++) {
        this->place_pixel(0, i, 0, brick);
        this->place_pixel(winx(), i, 0, brick);
    }

    for (int i = 0; i <= winx(); i++)
        for (int j = 0; j <= winy(); j++) {
            //printf("here in draw\n");
            win.draw(field[i][j]);
        }
}

void Window::on_key(event_fn fn) {
    events.push_back(fn);
}

void Window::on_timer(int t, timer_fn fn) {
    timers.emplace_back(std::max(0,t), fn);
    std::sort(timers.begin(), timers.end(),
            [] (const auto& a, const auto& b) { return a.first < b.first; } );
}

void Window::quit() {}

void Window::painter(const Segment& body) {
    Elem* snake = new Elem('s');
    int angle;
    if (body.dir <= 3) {
        snake->setTextureRect({0, 0, pixel_size, pixel_size});
        switch (body.dir) {
            case 0: angle = 0; break;
            case 1: angle = 2; break;
            case 2: angle = 1; break;
            case 3: angle = 3; break;
            default: break;
        }
    }
    else if (body.dir == Snake::Direction::BODY)
        snake->setTextureRect({pixel_size, 0, pixel_size * 2, pixel_size});
    else
        snake->setTextureRect({pixel_size * 2, 0, pixel_size * 3, pixel_size});

    this->place_pixel(body.first, body.second, angle, *snake);
}

void Window::painter(const Rabbit& xy) {
    Elem* rab = new Elem('r');
    this->place_pixel(xy.first, xy.second, 0, *rab);
}
void Window::painter(int, int) {}
void Window::painter(const char* str, bool highlight) {}

Window::~Window() {}

bool Window::read_event() {
           
    static int t = 1;
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) t = 1;
    if (t == 0) return false;
    int key;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) key = KEY_LEFT;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) key = KEY_RIGHT;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) key = KEY_UP;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) key = KEY_DOWN;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        key = ' ';
        t = 0;
        }

    for (auto& f : events)
        f(key);

    return false;
 }

void Window::run(Game& g) {
    

    
    //int ii = 0;
    

    running = true;
    while (running && win.isOpen()) {
        
        sf::Event event;
        while (win.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                win.close();
        }

        win.clear();
        draw();
        
        struct pollfd pfd[1] = {{ .fd = STDIN_FILENO, .events = POLLIN}};
        int msecs = -1;

        if (!timers.empty())
            msecs = timers.front().first;

        struct timespec ta, tb;
        clock_gettime(CLOCK_MONOTONIC, &ta);
        poll(pfd, 1, msecs);
        clock_gettime(CLOCK_MONOTONIC, &tb);
        int elapsed = ( (tb.tv_sec - ta.tv_sec) * 1000 + (tb.tv_nsec - ta.tv_nsec) / 1000000 );

        if (msecs != -1) {
            auto i = timers.begin();
            auto f = i->second;

            for (auto& t : timers)
                t.first = std::max(0, t.first - elapsed);

            if (msecs <= elapsed) {
                timers.erase(i);
                f();
            }
        }

            
        read_event();
        win.display();
    }
}