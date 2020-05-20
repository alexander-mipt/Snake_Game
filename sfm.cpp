#include "sfm.h"
 	
#include <sstream>

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
    //texture.setSrgb(true);
	this->setTexture(texture);
    //sf::IntRect size = this->getTextureRect();
    //printf("size = %d\n", size.width);
    //int scale = pixel_size / size.width; 
    //this->setScale(scale,scale);
}

Elem::~Elem() {
    //printf("Deleting Elem!\n");
    //delete this;
}

Window::Window() : pixel_size(P_SIZE) {

	set_screen_size();
	win.create(density, "Snake Game");
    //sf::Sprite sp;
    this->clock.restart();

    this->fnt.loadFromFile("arial.ttf");
    txt[0].second.setFont(this->fnt);
    txt[0].first = 1;
    this->txt[0].second.setString("Score: ");
    this->txt[0].second.setCharacterSize(FONT_SIZE);
    this->txt[0].second.setFillColor(sf::Color::White);
    for(int l = 1; l < TEXT_FIELDS; ++l) {
        txt[l].second.setFont(this->fnt);
        txt[l].second.setCharacterSize(FONT_SIZE);
        txt[l].first = 0;
    }

	/*for (int i = 0; i < 500; i++)
        for (int j = 0; j < 500; j++) {
            printf("here init\n");
            field[i][j] = sp;
        }*/
    

}

void Window::set_screen_size() {
    sf::VideoMode fullscreen = sf::VideoMode::getDesktopMode();
    int width = fullscreen.width - 25;
    int height = fullscreen.height - 50;
	width -= width % pixel_size;
	height -= height % pixel_size;
	//printf("w = %d, h = %d\n", width, height);
    density.width = width;
	density.height = height;
}



void Window::place_pixel(int x, int y, int angle, sf::Sprite& pixel) {

    x-=1;
    y-=1;

	pixel.setOrigin(pixel_size / 2, pixel_size / 2);
	pixel.setRotation(90. * angle);
	pixel.setPosition( (x + 0.5) * pixel_size , (y + 0.5) * pixel_size);
	//win.draw(pixel);
    field[x][y] = pixel;
	
}

sf::Color Window::int_to_color(unsigned int c) {
    if (c == 1)
        return sf::Color::Red;
    switch(c % 5) {
        case 0:
        return sf::Color::Magenta;
        case 1:
        return sf::Color::Green;
        case 2:
        return sf::Color::Yellow;
        case 3:
        return sf::Color::Cyan;
        case 4:
        return sf::Color::Blue;
    }

    return sf::Color::White;
}

void Window::draw() {
    Elem brick('w');
    
    for (int i = 1; i <= winx(); i++) {
        this->place_pixel(i, 1, 0, brick);
        this->place_pixel(i, winy(), 0, brick);
    }

    for (int i = 2; i <= winy() - 1; i++) {
        this->place_pixel(1, i, 0, brick);
        this->place_pixel(winx(), i, 0, brick);
    }

    for (int i = 1; i <= winx(); i++)
        for (int j = 1; j <= winy(); j++) {
            //printf("here in draw\n");
            win.draw(field[i-1][j-1]);
        }


    sf::Vector2f banner = this->txt[0].second.getPosition();
    sf::FloatRect score_rect = this->txt[0].second.getLocalBounds();
    for (int y = 0; y < TEXT_FIELDS - 2; ++y) {
        if (this->txt[y].first) {
            sf::FloatRect txt_rect = this->txt[y].second.getLocalBounds();
            if (y)
                this->txt[y].second.setPosition(banner.x + score_rect.width + y * 5 * FONT_SIZE, banner.y);
            sf::FloatRect txt_rect0 = this->txt[y].second.getLocalBounds();
            sf::RectangleShape background(sf::Vector2f(txt_rect0.width * 2, txt_rect0.height * 1.5));
            background.setFillColor(sf::Color::Black);
            this->win.draw(background, txt[y].second.getTransform());
            this->win.draw(txt[y].second);
        }
        
    }
    
}

void Window::on_key(event_fn fn) {
    events.push_back(fn);
}

void Window::on_timer(int t, timer_fn fn) {
    timers.emplace_back(std::max(0,t), fn);
    std::sort(timers.begin(), timers.end(),
            [] (const auto& a, const auto& b) { return a.first < b.first; } ); 
            // в порядке возрастания времени
}

void Window::quit() {
    int count = 0;
    for (auto i : this->rubbish) {
        if (i != nullptr) {
            delete i;
            i = nullptr;
            //printf("%d) Deleted Dynamic object.\n", ++count);
        }
        
        this->rubbish.clear();
    }
        
   
}

void Window::painter(const Segment& body) {
    Elem* snake = new Elem('s');
    this->rubbish.push_back(snake);
    snake->setColor(int_to_color(body.brand));
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
    this->rubbish.push_back(rab);
    this->place_pixel(xy.first, xy.second, 0, *rab);
}
void Window::painter(int brand, int score) {
    std::ostringstream snake_str;    // объявили переменную
	snake_str << score;
    
	this->txt[brand].second.setString(snake_str.str());
    this ->txt[brand].second.setFillColor(int_to_color(brand));
    this->txt[brand].first = 1;
	
}
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
    int kk = 0;
    running = true;
    while (running && win.isOpen()) {
         
        
        
        sf::Event event;
        while (win.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                this->quit();
                win.close();
            }
        }

        
        
        int msecs = -1;

        if (!timers.empty()) {
            // on_timer() adds timers
            msecs = timers.front().first; 
            // returns the shortest time interval before next action
        }

        win.clear();
        draw();
            
            this->clock.restart();
            while (this->clock.getElapsedTime().asMilliseconds() <= msecs) {
                
                read_event();
                //win.display();
            }

        //int elapsed = ( (tb.tv_sec - ta.tv_sec) * 1000 + (tb.tv_nsec - ta.tv_nsec) / 1000000 );

        //if there are some time pueue:
        if (msecs != -1) {
            auto i = timers.begin(); // first timer
            auto f = i->second; // timer_fn (time action type)

            for (auto& t : timers)
                t.first = std::max(0, t.first - this->clock.getElapsedTime().asMilliseconds()); // reduce clock on every timer

            if (1) { // if there are no key_pressed
                timers.erase(i);
                f();
            }
        }

            
        
        win.display();
       
    }
}