#ifndef _Music_hpp_
#define _Music_hpp_

#include "Internal/Sound.hpp"

#include <AL/al.h>
#include <sndfile.h>

#include <string>
#include <vector>

#include <memory>
#include <thread>
#include <mutex>

class Music : public Sound
{
    public:
        Music(const std::string& filename);
        ~Music();

        void play();
        void stop();

        void seek(float time);
        void relSeek(float time);
        float getDuration();
        float getTime();

        void setLoop(bool value);
        bool getLoop();

    private:
        struct SoundChunk
        {
            short* samples;
            std::size_t sampleCount;
        };

        // Load a Song from a file.
        // Uses libsndfile, so supported filetypes are: OGG, WAV, and FLAC.
        void loadSound(const std::string& filename);

    private:
        // Load a single chunk of data from the stream file into "c".
        bool loadChunk(SoundChunk& c);

    public:
        // Fill OpenAL buffers
        bool fillQueue();
        bool fillAndPushBuffer(unsigned int bufferNum);
        void clearQueue();

        // Getters and setters that need to be visible for the streamData function.
        void setStream(bool value);
        const bool isStreaming() const;
        ALint buffersProcessed();

        unsigned int popBuffer();
        unsigned int getBufferNum(ALuint buffer);

        void setSamplesProcessed(unsigned long i);
        void addSamplesProcessed(unsigned long i);
        unsigned long getSamplesProcessed();

        void setEndBuffer(unsigned int bufferNum, bool value);
        bool getEndBuffer(unsigned int bufferNum);

        // Threads run this function to load audio data
        static void streamData(Music* m);

    private:
        static const unsigned int BUFFER_COUNT = 4;

        SNDFILE* file;

        std::unique_ptr<std::thread> streamThread;
        bool streaming;
        unsigned int buffers[BUFFER_COUNT];
        bool endBuffers[BUFFER_COUNT];

        bool loop;

        std::size_t  sampleCount;
        unsigned int channelCount;
        unsigned int sampleRate;

        ALenum format;
        unsigned long samplesProcessed;

        float duration;

        std::vector<short> buffer;
};

#endif // _Music_hpp_
