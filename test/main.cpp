#include "Listener.hpp"
#include "Sample.hpp"
#include "Music.hpp"

#include <iostream>

#include <chrono>
#include <thread>

#include <string>

int main(int argc, char* argv[])
{
    Listener::init();

    // Sample sample1("data/Coin.wav");
    // Sample sample2("data/Powerup.wav");
    // Sample sample3("data/Randomize.wav");
    Music music("data/hds.flac");

    music.setVolume(0.7f);
    music.play();

    while (music.isStreaming())
    {
        printf("%f / %f\n", music.getTime(), music.getDuration());
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    Listener::deinit();
    return 0;
}
