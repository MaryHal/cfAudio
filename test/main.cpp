#include "Listener.hpp"
#include "Sample.hpp"
#include "Music.hpp"

#include <chrono>
#include <thread>

#include <memory>
#include <cstdio>

int main(int argc, char* argv[])
{
    cfAudio::Listener::init();

    // std::unique_ptr<Sound> sample(new Sample("data/Coin.wav"));
    // Sample sample1("data/Coin.wav");
    // Sample sample2("data/Powerup.wav");
    // Sample sample3("data/Randomize.wav");

    if (argc > 1)
    {
        std::string songFile = argv[1];
        printf("File: %s\n", songFile.c_str());

        std::unique_ptr<cfAudio::Sound> music(new cfAudio::Music(songFile));
        music->play();

        while (!music->isStopped())
        {
            printf("%.2f / %.2f\n", music->getTime(), music->getDuration());
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    else
    {
        printf("No arguments provided. Give me a music file! (ogg, wav, flac)\n");
    }

    cfAudio::Listener::deinit();
    return 0;
}
