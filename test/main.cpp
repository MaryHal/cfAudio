#include "Listener.hpp"
#include "Sample.hpp"
#include "Music.hpp"

#include <chrono>
#include <thread>

#include <memory>
#include <cstdio>

int main(int argc, char* argv[])
{
    Listener::init();

    // std::unique_ptr<Sound> sample(new Sample("data/Coin.wav"));
    // Sample sample1("data/Coin.wav");
    // Sample sample2("data/Powerup.wav");
    // Sample sample3("data/Randomize.wav");

    // std::string songFile = "data/hds.flac";
    std::unique_ptr<Sound> music;
    if (argc > 1)
    {
        songFile = argv[1];
    }
    else
    {
        printf("No arguments provided. Give me a music file! (ogg, wav, flac)");
        return 0;
    }

    printf("File: %s\n", songFile.c_str());

    music.reset(new Music(songFile));

    // music->seek(330);
    // music->setLoop(true);
    music->play();

    while (!music->isStopped())
    {
        printf("%.2f / %.2f\n", music->getTime(), music->getDuration());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    Listener::deinit();
    return 0;
}
