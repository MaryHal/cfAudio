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
        float getTime();
        float getDuration();

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
        // Returns true if load was successful, false if next chunk cannot be loaded
        // (due to error, or if there is no more data to load).
        bool loadChunk(SoundChunk& c);

    public:
        // Fill/Clear ALL OpenAL buffers
        bool fillQueue();
        void clearQueue();

        // Fill the index'th buffer.
        bool fillBuffer(unsigned int index);

        // Getters and setters that need to be visible for the streamData function.
        void setStream(bool value);
        const bool isStreaming() const;

        // Returns the number of processed buffers. I.e., the number of buffers that need
        // to be filled with new data.
        ALint buffersProcessed();

        // Returns the buffer handle for an unused buffer.
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

        std::mutex threadMutex;
        std::unique_ptr<std::thread> streamThread;
        bool streaming;
        ALuint buffers[BUFFER_COUNT];  // OpenAL buffer handles
        bool endBuffers[BUFFER_COUNT]; // Buffer finished flags

        // Since we're linking many buffers to a single source and refilling
        // these buffers periodically, we can't use the built-in OpenAL loop functionality.
        bool loop;

        // Sound file data
        std::size_t  sampleCount;
        unsigned int channelCount;
        unsigned int sampleRate;

        ALenum format;
        unsigned long samplesProcessed;

        float duration;

        std::vector<short> buffer;
};

#endif // _Music_hpp_
