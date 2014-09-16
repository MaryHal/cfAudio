cfAudio
=======

Simple C++11 Audio Library. Built on top of OpenAL and libsndfile.

Basic Usage
===========

Be sure to link cfAudio64 (or cfAudio32 for 32-bit systems) and make sure your compiler can find the header files in the `include` directory.

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

Playing OGG files results in some crackling noises in the background. From what I've researched, this seems to be a libsndfile problem.

Perhaps in the future, I'll switch from libsndfile to [libavformat](https://www.ffmpeg.org/libavformat.html) and [libavcodec](https://www.ffmpeg.org/libavcodec.html)
