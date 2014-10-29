#ifndef _SoundBuffer_h_
#define _SoundBuffer_h_

#include <AL/al.h>

#include "../Memory/Buffer.hpp"
#include <string>

namespace cfAudio
{
    class SoundBuffer : public Buffer
    {
        private:
            ALuint buffer;

            size_t  sampleCount;
            unsigned int channelCount;
            unsigned int sampleRate;

        public:
            SoundBuffer();
            SoundBuffer(const std::string& filename);
            ~SoundBuffer();

            void loadFromFile(const std::string& filename);

            void update();
            unsigned int getBufferID() const;

            size_t getSampleCount() const;
    };
}

#endif
