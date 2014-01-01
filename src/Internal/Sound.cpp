#include "Sound.hpp"
#include "SoundLoader.hpp"

#include "../System/Log.hpp"

void Sound::__generateSource()
{
    // Setup our source
    alGenSources(1, &source);
    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 0.5);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_FALSE);
}

void Sound::__setSource(const unsigned int& bufferID)
{
    alSourcei(source, AL_BUFFER, bufferID);
}

Sound::Sound()
    : status(Stopped),
    source(0)
{
}

Sound::Sound(const std::string& filename)
    : status(Stopped),
    source(0)
{
    Console::logf("Sound @ \"%s\" loaded.", filename.c_str());
}

Sound::~Sound()
{
    alDeleteSources(1, &source);
}

void Sound::play()
{
    Console::logf("Play source: %d", source);
    status = Playing;
    alSourcePlay(source);
}

void Sound::stop()
{
    status = Stopped;
    alSourceStop(source);
}

void Sound::pause()
{
    if (status == Paused)
    {
        play();
    }
    else if (status == Playing)
    {
        status = Paused;
        alSourcePause(source);
    }
}

void Sound::setVolume(float volume)
{
    alSourcef(source, AL_GAIN, volume);
}

float Sound::getVolume()
{
    ALfloat volume;
    alGetSourcef(source, AL_GAIN, &volume);

    return volume;
}

void Sound::setLoop(bool loop)
{
    alSourcei(source, AL_LOOPING, loop);
}

bool Sound::getLoop() const
{
    ALint loop;
    alGetSourcei(source, AL_LOOPING, &loop);

    return loop != 0;
}

bool Sound::isPlaying()
{
    return status == Playing;
}

bool Sound::isPaused()
{
    return status == Paused;
}

void Sound::setTime(float time)
{
}

float Sound::getTime()
{
    return 0;
}

float Sound::getDuration()
{
    return 0;
}

ALuint Sound::getSource()
{
    return source;
}

Status Sound::getStatus()
{
    return status;
}
