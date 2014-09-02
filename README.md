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


Issues
===========

Currently, seeking while a song is playing/streaming results in varying behavior based on filetype. WAV files seem to work okay, OGG files segfault every time, and FLAC files sometimes hang indefinitely. Backtraces lead me to malloc errors within libsndfile...sometimes.

Playing OGG files results in some crackling noises in the background. From what I've researched, this seems to be a libsndfile problem.
