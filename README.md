cfAudio
=======

Simple C++11 Audio Library. Built on top of OpenAL and libsndfile.

Basic Usage
===========

Initialize Listener:

    Listener::init();

Playing Music:

    Music music("data/Melodica.ogg");
    music.play();
    music.pause();
    music.setVolume(0.5f);

Playing a Sample:

    Sample sample1("data/Coin.wav");
    sample1.play();

    Sample sample2("data/Powerup.wav");
    sample2.play();

Deinitialize Listener when done:

    Listener::deinit();

