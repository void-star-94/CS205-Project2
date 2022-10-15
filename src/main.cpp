#include "Number.h"
#include <algorithm>
#include <ncurses.h>
#include <cstring>

uint64_t Number::precision = 32;
Print_Tag Number::tag = Print_Tag::Scientific;

/*
int scanKeyboard() {
    int input;
    termios new_settings{}, stored_settings{};
    tcgetattr(0, &stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    tcgetattr(0, &stored_settings);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &new_settings);
    input = getchar();
    tcsetattr(0, TCSANOW, &stored_settings);
    return input;
}
*/


int main() {
    /*
    uint64_t tmp = 114514;
//    Number::tag = Print_Tag::Decimal;
    Number a(114514);
    Number b(1919810);
    Number re = a / b;
    re.squeeze();
//    a -= b;
    std::cout << (b % a).first;
    std::cout << "\n------------------------\n";
    std::cout << (b / a) << "\n";
*/
    initscr();
    raw();
    noecho();
    curs_set(0);

    const char *c = "Hello, World!";

    mvprintw(LINES / 2, int(COLS - strlen(c)) / 2, c);
    refresh();

    getch();
    endwin();

    return 0;
}
