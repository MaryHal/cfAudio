#include "Listener.hpp"
#include "Sample.hpp"
#include "Music.hpp"

// #include <chrono>
// #include <thread>

#include <iostream>
#include <memory>

int main(int argc, char* argv[])
{
    Listener::init();

    std::unique_ptr<Sound> sample(new Sample("data/Coin.wav"));
    // Sample sample1("data/Coin.wav");
    // Sample sample2("data/Powerup.wav");
    // Sample sample3("data/Randomize.wav");

    // std::unique_ptr<Sound> music(new Music("data/Melodica.ogg"));

    // music->setVolume(0.7f);
    // music->play();

    // printf("%f / %f\n", music.getTime(), music.getDuration());

    sample->setVolume(1.0f);
    for (float x = -1.0f; x < 1.0f; x += 0.1f)
    {
        sample->setPan(x);

        std::cout << sample->getPan() << std::endl;
        sample->play();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    Listener::deinit();
    return 0;
}
