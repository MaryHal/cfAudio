#include "Music.hpp"

#include "Internal/SoundLoader.hpp"
#include "System/Log.hpp"

void Music::streamData(Music* m)
{
    bool requestStop = m->fillQueue();
    m->play();

    while (m->isStreaming())
    {
        log("Streaming...");
        ALint nbProcessed = m->buffersProcessed();

        while (nbProcessed--)
        {
            ALuint buffer = m->popBuffer();
            unsigned int bufferNum = m->getBufferNum(buffer);

            // Retrieve its size and add it to the samples count
            if (m->getEndBuffer(bufferNum))
            {
                // This was the last buffer: reset the sample count
                m->setSamplesProcessed(0);
                m->setEndBuffer(bufferNum, false);
            }
            else
            {
                ALint size, bits;
                alGetBufferi(buffer, AL_SIZE, &size);
                alGetBufferi(buffer, AL_BITS, &bits);
                m->addSamplesProcessed(size / (bits / 8));
            }

            // Fill it and push it back into the playing queue
            if (!requestStop)
            {
                if (m->fillAndPushBuffer(bufferNum))
                    requestStop = true;
            }
        }

        // Leave some time for the other threads if the stream is still playing
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    log("Stopping...");
    // Stop the playback
    m->stop();

    log("Clearing Queue...");
    // Unqueue any buffer left in the queue
    m->clearQueue();

    log("Music Stream Thread Killed");
}

Music::Music()
{
}

Music::Music(const std::string& filename)
    : file(NULL),
      streamThread(nullptr),
      streaming(false),
      loop(false),
      sampleCount(0),
      channelCount(0),
      sampleRate(0),
      samplesProcessed(0)
{
    // Generate Buffers
    alGenBuffers(BUFFER_COUNT, buffers);
    for (unsigned int i = 0; i < BUFFER_COUNT; ++i)
        endBuffers[i] = false;

    __generateSource();
    loadSound(filename);
}

Music::~Music()
{
    stop();

    alDeleteBuffers(BUFFER_COUNT, buffers);
    if (file)
        sf_close(file);
}

void Music::play()
{
    if (!streaming)
    {
        for (unsigned int i = 0; i < BUFFER_COUNT; ++i)
            endBuffers[i] = false;

        streaming = true;
        samplesProcessed = 0;
        seek(0);
        streamThread.reset(new std::thread(&Music::streamData, this));
    }
    else
    {
        Sound::play();
    }
}

void Music::stop()
{
    if (status != Stopped)
    {
        Sound::stop();
        streaming = false;
        streamThread->join();

        samplesProcessed = 0;
    }
}

void Music::seek(float time)
{
    std::mutex threadMutex;
    threadMutex.lock();

    if (file)
    {
        sf_count_t frameOffset = static_cast<sf_count_t>(time * 1000 * sampleRate);
        sf_seek(file, frameOffset, SEEK_SET);
    }

    threadMutex.unlock();
}

float Music::getDuration()
{
    return duration;
}

float Music::getTime()
{
    ALfloat secs = 0.f;
    alGetSourcef(source, AL_SEC_OFFSET, &secs);

    return secs + static_cast<float>(samplesProcessed) / sampleRate / channelCount;
}

void Music::setLoop(bool value)
{
    loop = value;
}

bool Music::getLoop()
{
    return loop;
}

void Music::loadSound(const std::string& filename)
{
    SF_INFO FileInfos;
    file = sf_open(filename.c_str(), SFM_READ, &FileInfos);
    if (!file)
    {
        //std::cerr << "Failed to read sound file \"" << Filename << "\"" << std::endl;
        return;
    }

    // Set the sound parameters
    sampleCount = static_cast<std::size_t>(FileInfos.frames) * FileInfos.channels;
    channelCount = FileInfos.channels;
    sampleRate = FileInfos.samplerate;

    format = SoundLoader::getFormat(channelCount);

    // Check if the format is valid
    if (format == 0)
    {
        channelCount = 0;
        sampleRate   = 0;

        logf("Unsupported number of channels (%d)", channelCount);
    }

    duration = static_cast<float>(sampleCount) / sampleRate / channelCount;

    buffer.resize(sampleRate * channelCount);
}

bool Music::loadChunk(SoundChunk& c)
{
    std::mutex threadMutex;
    threadMutex.lock();

    c.samples = &buffer[0];

    if (file && c.samples && sampleCount)
        c.sampleCount = static_cast<std::size_t>(sf_read_short(file, &buffer[0], buffer.size()));
    else
        c.sampleCount = 0;

    threadMutex.unlock();

    return c.sampleCount == buffer.size();
}

bool Music::fillQueue()
{
    // Fill and enqueue all the available buffers
    bool requestStop = false;
    for (unsigned int i = 0; (i < BUFFER_COUNT) && !requestStop; ++i)
    {
        if (fillAndPushBuffer(i))
            requestStop = true;
    }

    return requestStop;
}

bool Music::fillAndPushBuffer(unsigned int bufferNum)
{
    bool requestStop = false;

    // Acquire audio data
    SoundChunk chunk = {NULL, 0};
    if (!loadChunk(chunk))
    {
        endBuffers[bufferNum] = true;

        if (loop)
        {
            logf("looping");
            seek(0);

            if (!chunk.samples || chunk.sampleCount == 0)
            {
                fillAndPushBuffer(bufferNum);
            }
        }
        else
        {
            requestStop = true;
        }
    }

    // Buffer the data
    if (chunk.samples && chunk.sampleCount)
    {
        unsigned int buffer = buffers[bufferNum];

        // Fill the buffer
        ALsizei alsize = static_cast<ALsizei>(chunk.sampleCount) * sizeof(short);
        alBufferData(buffer, format, chunk.samples, alsize, sampleRate);

        // Push it into the sound queue
        alSourceQueueBuffers(source, 1, &buffer);
    }

    return requestStop;
}

void Music::clearQueue()
{
    log("Getting queued buffer count");
    // Get the number of buffers still in the queue
    ALint nbQueued;
    alGetSourcei(source, AL_BUFFERS_QUEUED, &nbQueued);

    log("Unqueuing");
    // Unqueue them all
    ALuint buffer;
    for (ALint i = 0; i < nbQueued; ++i)
        alSourceUnqueueBuffers(source, 1, &buffer);
}

void Music::setStream(bool value)
{
    streaming = value;
}

const bool Music::isStreaming() const
{
    return streaming;
}

ALint Music::buffersProcessed()
{
    ALint processed = 0;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

    return processed;
}

unsigned int Music::popBuffer()
{
    ALuint unusedBuffer;
    alSourceUnqueueBuffers(source, 1, &unusedBuffer);

    return unusedBuffer;
}

unsigned int Music::getBufferNum(ALuint buffer)
{
    // Find its number
    unsigned int bufferNum = 0;
    for (unsigned int i = 0; i < BUFFER_COUNT; ++i)
    {
        if (buffers[i] == buffer)
        {
            bufferNum = i;
            break;
        }
    }

    return bufferNum;
}

void Music::setSamplesProcessed(unsigned long i)
{
    samplesProcessed = i;
}

void Music::addSamplesProcessed(unsigned long i)
{
    samplesProcessed += i;
}

unsigned long Music::getSamplesProcessed()
{
    return samplesProcessed;
}

void Music::setEndBuffer(unsigned int bufferNum, bool value)
{
    endBuffers[bufferNum] = value;
}

bool Music::getEndBuffer(unsigned int bufferNum)
{
    return endBuffers[bufferNum];
}
