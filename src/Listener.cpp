#include <Listener.hpp>

#include "System/Log.hpp"

namespace cfAudio
{
    ALCdevice* Listener::device = nullptr;
    ALCcontext* Listener::context = nullptr;

    void Listener::init()
    {
        device = alcOpenDevice(nullptr);
        if (device)
        {
            context = alcCreateContext(device, nullptr);
            alcMakeContextCurrent(context);
        }

        // Let's keep 2d sound for now.
        alListener3f(AL_POSITION, 0, 0, 0);
        alListener3f(AL_VELOCITY, 0, 0, 0);
        alListener3f(AL_ORIENTATION, 0, 0, -1);

        Console::log("Listener Initialized.");
    }

    void Listener::deinit()
    {
        alcMakeContextCurrent(nullptr);
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
} // namespace cfAudio
