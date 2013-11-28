#include "Listener.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include "System/Log.hpp"

ALCdevice* Listener::device = NULL;
ALCcontext* Listener::context = NULL;

void Listener::init()
{
    device = alcOpenDevice(NULL);
    if (device)
    {
        context = alcCreateContext(device, NULL);
        alcMakeContextCurrent(context);
    }

    // Let's keep 2d sound for now.
    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    alListener3f(AL_ORIENTATION, 0, 0, -1);

    log("Listener Initialized.");
}

void Listener::deinit()
{
    alcMakeContextCurrent(NULL);
    if (context)
        alcDestroyContext(context);
    if (device)
        alcCloseDevice(device);
}

void Listener::setVolume(float volume)
{
    alListenerf(AL_GAIN, volume);
}

float Listener::getVolume()
{
    float volume = 0.f;
    alGetListenerf(AL_GAIN, &volume);

    return volume;
}



