#ifndef _Music_hpp_
#define _Music_hpp_

#include "Sound.hpp"

#include <AL/al.h>

#include <sndfile.h>

#include <string>
#include <vector>

class Music : public Sound
{
private:
    struct SoundChunk
    {
        short* samples;
        std::size_t sampleCount;
    };

    static const unsigned int BUFFER_COUNT = 5;

    SNDFILE* file;

    // GLFWthread thread;
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

public:
    Music();
    Music(const std::string& filename);
    ~Music();

    void play();
    void stop();

    void seek(float time);
    float getDuration();
    float getTime();

    void setLoop(bool value);
    bool getLoop();

    void loadSound(const std::string& filename);
    bool loadChunk(SoundChunk& c);

    bool fillQueue();
    bool fillAndPushBuffer(unsigned int bufferNum);
    void clearQueue();

    void setStream(bool value);
    bool isStreaming();
    ALint buffersProcessed();

    unsigned int popBuffer();
    unsigned int getBufferNum(ALuint buffer);

    void setSamplesProcessed(unsigned long i);
    void addSamplesProcessed(unsigned long i);
    unsigned long getSamplesProcessed();

    void setEndBuffer(unsigned int bufferNum, bool value);
    bool getEndBuffer(unsigned int bufferNum);
};

#endif
