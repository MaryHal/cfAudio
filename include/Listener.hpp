#ifndef _Listener_hpp_
#define _Listener_hpp_

#include <AL/al.h>
#include <AL/alc.h>

namespace cfAudio
{
    class Listener
    {
        private:
            // typedef struct ALCdevice_struct ALCdevice;
            // typedef struct ALCcontext_struct ALCcontext;

            static ALCdevice* device;
            static ALCcontext* context;

        public:
            // Initialize our OpenAL device/context.
            // Listener must be initialized before usage of Music and Sample objects.
            static void init();
            static void deinit();

            // Set virtual device(global) volume
            static void setVolume(float volume);
            static float getVolume();
    };
}

#endif // _Listener_hpp_
