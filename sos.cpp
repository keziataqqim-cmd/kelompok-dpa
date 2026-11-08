#include <ncurses.h>

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    mvprintw(0, 0, "Halo aku");
    mvprintw(2, 0, "Tekan tombol apa saja untuk keluar...");
    refresh();

    getch();
    endwin();
    return 0;
}
