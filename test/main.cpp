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

    music->play();
    music->setVolume(0.1f);
    music->seek(330.0f);

    while (music->isPlaying())
    {
        std::cout << music->getTime() << " / " << music->getDuration() << std::endl;
        if (music->getStatus() == Stopped)
        {
            std::cout << "Stopped?" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // printf("%f / %f\n", music.getTime(), music.getDuration());

    // sample->setVolume(1.0f);
    // for (float x = -1.0f; x < 1.0f; x += 0.1f)
    // {
    //     sample->setPan(x);

    //     std::cout << sample->getPan() << std::endl;
    //     sample->play();
    //     std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // }

    Listener::deinit();
    return 0;
}
