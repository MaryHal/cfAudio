#include <Music.hpp>
#include <Internal/SoundLoader.hpp>

#include "System/Log.hpp"

#include <chrono>

Music::Music(const std::string& filename)
    : Sound(),
      file(NULL),
      streamThread(nullptr),
      streaming(false),
      loop(false),
      sampleCount(0),
      channelCount(0),
      sampleRate(0),
      samplesProcessed(0)
{
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

        // Technically, no OpenAL sources will be playing yet (still in the
        // loading phase), but the user, upon calling a "play" function
        // would (and should) expect the status of the song to be "playing".
        status = Playing;

        streaming = true;
        samplesProcessed = 0;
        // seek(0);
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

        seek(0.0);
        clearQueue();

        samplesProcessed = 0;
    }
}

void Music::seek(float time)
{
    threadMutex.lock();

    if (file)
    {
        // clearQueue();
        sf_count_t offset = static_cast<sf_count_t>(time * sampleRate);
        int code = sf_seek(file, offset, SEEK_SET);

        int error = sf_error(file);
        if (error == 0)
        {
            Console::logf("seek %d", code);
        }
        else
        {
            Console::logf("libsndfile error(%d): %s", error, sf_strerror(file));
        }
    }

    threadMutex.unlock();
}

void Music::relSeek(float time)
{
    float targetTime = getTime() + time;
    seek(targetTime);
}

float Music::getTime()
{
    ALfloat secs = 0.f;
    alGetSourcef(source, AL_SEC_OFFSET, &secs);

    return secs + static_cast<float>(samplesProcessed) / sampleRate / channelCount;
}

float Music::getDuration()
{
    return duration;
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
    stop();

    // Generate Buffers
    alGenBuffers(BUFFER_COUNT, buffers);
    for (unsigned int i = 0; i < BUFFER_COUNT; ++i)
        endBuffers[i] = false;
    __generateSource();

    SF_INFO FileInfos;
    file = sf_open(filename.c_str(), SFM_READ, &FileInfos);
    if (!file)
    {
        Console::logf("Failed to load \"%s\".", filename.c_str());
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

        Console::logf("Unsupported number of channels (%d).", channelCount);
    }

    duration = static_cast<float>(sampleCount) / sampleRate / channelCount;

    buffer.resize(sampleRate * channelCount);
}

bool Music::loadChunk(SoundChunk& c)
{
    threadMutex.lock();

    c.samples = &buffer[0];

    if (file != nullptr && c.samples && sampleCount)
        c.sampleCount = static_cast<std::size_t>(sf_read_short(file, &buffer[0], buffer.size()));
    else
        c.sampleCount = 0;

    threadMutex.unlock();

    // The sf_read_XXXX functions return the number of items read.
    // Unless the end of the file was reached during the read,
    // the return value should equal the number of items requested.
    // Attempts to read beyond the end of the file will not result in
    // an error but will cause the sf_read_XXXX functions to return
    // less than the number of items requested or 0 if already at the end of the file.

    // Are we at the end of the audio file?
    return c.sampleCount == buffer.size();
}

bool Music::fillQueue()
{
    // Fill and enqueue all the available buffers
    bool requestStop = false;
    for (unsigned int i = 0; (i < BUFFER_COUNT && !requestStop); ++i)
    {
        if (fillBuffer(i))
            requestStop = true;
    }

    return requestStop;
}

bool Music::fillBuffer(unsigned int index)
{
    bool requestStop = false;

    // Acquire audio data
    SoundChunk chunk = {NULL, 0};

    if (!loadChunk(chunk))
    {
        // Mark this buffer as the last buffer of the file.
        endBuffers[index] = true;

        if (loop)
        {
            // Seek to the beginning of the song and load the final buffer.
            seek(0);

            if (!chunk.samples || chunk.sampleCount == 0)
            {
                return fillBuffer(index);
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
        unsigned int buffer = buffers[index];

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
    // Get the number of buffers still in the queue
    ALint nbQueued;
    alGetSourcei(source, AL_BUFFERS_QUEUED, &nbQueued);

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

void Music::streamData(Music* m)
{
    bool requestStop = m->fillQueue();
    m->play();

    while (!requestStop && m->isStreaming())
    {
        ALint processedCount = m->buffersProcessed();

        while (processedCount--)
        {
            // Get an empty buffer
            ALuint buffer = m->popBuffer();
            unsigned int bufferNum = m->getBufferNum(buffer);

            // If this buffer is the final buffer for the file, reset the sample count
            // to prepare for possible music looping.
            if (m->getEndBuffer(bufferNum))
            {
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
                if (m->fillBuffer(bufferNum))
                    requestStop = true;
            }
        }

        // Leave some time for the other threads if the stream is still playing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    m->stop();

    // Unqueue any buffer left in the queue
    m->clearQueue();
}
