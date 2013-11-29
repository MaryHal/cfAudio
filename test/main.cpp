#include "Listener.hpp"
#include "Sample.hpp"
#include "Music.hpp"

#include <curses.h>

int main(int argc, char* argv[])
{
    Listener::init();

    Sample sample1("data/Coin.wav");
    Sample sample2("data/Powerup.wav");
    Sample sample3("data/Randomize.wav");
    Music music("data/Melodica.ogg");

    // Curses initialization
    initscr();
    cbreak();
    noecho();
    // nodelay(stdscr, true);
    keypad(stdscr, true);

    for (;;)
    {
        clear();

        // Draw
        waddstr(stdscr, "Press a to play, q to quit. 1, 2, and 3 play various sounds.");
        wrefresh(stdscr);

        // Handle Input
        int ch = getch();
        if (ch == 'q') // Escape or alt
        {
            break;
        }
        else if (ch == 'a')
        {
            music.play();
        }
        else if (ch == '1')
        {
            sample1.play();
        }
        else if (ch == '2')
        {
            sample2.play();
        }
        else if (ch == '3')
        {
            sample3.play();
        }
    }

    // Curses cleanup
    endwin();

    Listener::deinit();
    return 0;
}

