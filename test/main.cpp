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

    std::unique_ptr<Sound> music(new Music("data/Melodica.ogg"));

    music->setVolume(0.7f);
    music->play();

    for (;;)
    {
        std::string input;
        std::cout << " >> ";
        std::getline(std::cin, input);

        if (input == "P")
            {
                music->play();
            }
        else if (input == "p")
            {
                music->pause();
            }
        else if (input == "S")
            {
                music->stop();
            }
        else if (input == "a")
            {
                sample->play();
            }
        else if (input == "q")
            {
                music->stop();
                break;
            }

        // printf("%f / %f\n", music.getTime(), music.getDuration());
        // std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    Listener::deinit();
    return 0;
}
