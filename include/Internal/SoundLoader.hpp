#ifndef _SoundLoader_hpp_
#define _SoundLoader_hpp_

#include <AL/al.h>
#include "SoundBuffer.hpp"

namespace cfAudio
{
    class SoundLoader
    {
        public:
            // From Channel count, return the OpenAL audio format
            static ALenum getFormat(unsigned int channels);
    };
}

#endif
