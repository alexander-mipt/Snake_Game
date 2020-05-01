#include "tui.h"
#include <cstdio>
#include <sys/poll.h>

Tty::Tty() {
  init_tty(0);
  winch();
  setvbuf(stdout, NULL, _IONBF, 0);
}

Tty::~Tty() {
  cls();
  fini_tty();
  setvbuf(stdout, NULL, _IOLBF, 0);
  printf("bye!\n");
}

void Tty::cls() {
  printf("\e[H\e[J\e[m");
}

void Tty::gotoxy(int x, int y) {
  printf("\e[%d;%dH", ws.ws_row - y, x + 1);
}

void Tty::setcolor(int c) {
  printf("\e[%dm", c ? c + 30 : 0);
}

void Tty::painter(int brand, int score) {
    gotoxy(10 * brand, 0);
    setcolor(brand);
    printf(" %d ", score);
    setcolor(0);
    gotoxy(0, 0);
}

void Tty::painter(const Segment& s) {
    setcolor(s.brand);
    putc(s.first, s.second, "^v><# "[s.dir]);
    setcolor(0);
    gotoxy(0, 0);
}

void Tty::painter(const Rabbit& s) {
    putc(s.first, s.second, '&');
    gotoxy(0, 0);
}

void Tty::painter(const char* str, bool highlight) {
    int center = winx() / 2 - sizeof(str)*1.5;
    if (highlight)
        printf("\e[5m");
    else
        printf("\e[25m");
    
    printf("\e[%d;%dH|    %s    |", 1, center, str);
    gotoxy(0,0);
}

void Tty::draw() {
    winch();

    cls();

    vline(1, 1, winy());
    hline(1, 1, winx());
    vline(winx(), 1, winy());
    hline(1, winy(), winx());

    gotoxy(3,0);
    printf(" Score: ");
    painter("G a m e __ P a u s e d (Press SPACE)", true);

    gotoxy(0,0);
}


void Tty::putc(int x, int y, char c) {
    printf("\e[%d;%dH%c", y, x, c);
}

void Tty::hline(int x, int y, int len) {
    for (int i = x; i <= len; i++) {
      putc(i, y, 'S');
    }
}

void Tty::vline(int x, int y, int len) {
    for (int i = y; i <= len; i++) {
      putc(x, i, 'S');
    }
}



static void on_winch(int signo) {
    Ui::get()->draw();
}

static void on_term(int signo) {
    Ui::get()->quit();
}

void Tty::init_tty(int echo) {
    struct termios tty;

    tcgetattr(0, &tty);
    ttys = tty;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= echo ? ECHO : ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);

    signal(SIGINT, on_term);
    signal(SIGTERM, on_term);
    signal(SIGWINCH, on_winch);
}

void Tty::fini_tty(void) {

    tcsetattr(STDIN_FILENO, TCSANOW, &ttys);
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGWINCH, SIG_DFL);
}

void Tty::winch() {
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
}

void Tty::quit() {
    running = false;
}

void Tty::on_key(event_fn fn) {
    events.push_back(fn);
}

void Tty::on_timer(int t, timer_fn fn) {
    timers.emplace_back(std::max(0,t), fn);
    std::sort(timers.begin(), timers.end(),
            [] (const auto& a, const auto& b) { return a.first < b.first; } );
}

void Tty::run(Game& g) {
    game = &g;

    draw();

    running = true;
    while (running) {
        struct pollfd pfd[1] = {{ .fd = STDIN_FILENO, .events = POLLIN}};
        int msecs = -1;

        if (!timers.empty())
            msecs = timers.front().first;

        struct timespec ta, tb;
        clock_gettime(CLOCK_MONOTONIC, &ta);
        int n = poll(pfd, 1, msecs);
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

        if (n == 1)
            if (read_event())
                break;
    }
}

// BETA -- READY
bool Tty::read_event() {
    char ch[100];
    int n = read(STDIN_FILENO, ch, sizeof(ch));

    if (n <= 0)
        return true;

    if (n == 1 && ch[0] == '\e')
        return true;

    for (int i = 0; i < n; i++) {
        int key = ch[i];

        if (ch[i] == '\e' && i + 2 < n && ch[i+1] == '[') {
            i += 2;
            switch (ch[i]) {
                case 'A': key = KEY_UP; break;
                case 'B': key = KEY_DOWN; break;
                case 'C': key = KEY_RIGHT; break;
                case 'D': key = KEY_LEFT; break;
            }
        }

        for (auto& f : events)
            f(key);
    }

    return false;
}
