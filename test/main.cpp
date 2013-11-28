#include "Listener.hpp"
#include "Sample.hpp"
#include "Music.hpp"

#include <iostream>

#include <chrono>
#include <thread>

int main(int argc, char* argv[])
{
    Listener::init();

    Sample sample1("data/Coin.wav");
    sample1.play();
    // Sample sample2("data/Powerup.wav");
    // sample2.play();
    Music music("data/Melodica.ogg");
    music.play();

    std::this_thread::sleep_for(std::chrono::milliseconds(30000));

    std::cout << "Done" << std::endl;

    Listener::deinit();
    return 0;
}
