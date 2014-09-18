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

    // std::unique_ptr<Sound> sample(new Sample("data/Coin.wav"));
    // Sample sample1("data/Coin.wav");
    // Sample sample2("data/Powerup.wav");
    // Sample sample3("data/Randomize.wav");

    std::unique_ptr<Sound> music(new Music("data/Melodica.ogg"));
    // std::unique_ptr<Sound> music(new Music("data/hds.flac"));

    music->setLoop(true);
    music->play();
    music->seek(340.0f);

    while (music->isPlaying())
    {
        std::cout << music->getTime() << " / " << music->getDuration() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    Listener::deinit();
    return 0;
}
