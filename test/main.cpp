#include "Listener.hpp"
#include "Sample.hpp"
#include "Music.hpp"

#include <chrono>
#include <thread>

int main(int argc, char* argv[])
{
    Listener::init();

    // Sample sample1("data/Coin.wav");
    // Sample sample2("data/Powerup.wav");
    // Sample sample3("data/Randomize.wav");
    Music music("data/Melodica.ogg");

    music.play();

    for (;;)
    {
        printf("%f / %f\n", music.getTime(), music.getDuration());
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    Listener::deinit();
    return 0;
}

