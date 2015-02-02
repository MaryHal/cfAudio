#include <Music.hpp>
#include <Internal/SoundLoader.hpp>

#include "System/Log.hpp"

#include <chrono>

namespace cfAudio
{
    Music::Music(const std::string& filename)
        : Sound(),
          file(nullptr),
          streamThread(nullptr),
          streaming(false),
          loop(false),
          sampleCount(0),
          channelCount(0),
          sampleRate(0),
          samplesProcessed(0),
          duration(0.0f)
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
            {
                std::lock_guard<std::mutex> lock(threadMutex);

                for (auto& elem : finalBuffer)
                    elem = false;

                // Technically, no OpenAL sources will be playing yet (still in the
                // loading phase), but the user, upon calling a "play" function
                // would (and should) expect the status of the song to be "playing".
                status = Playing;

                streaming = true;
            }

            seek(0.0f);

            streamThread.reset(new std::thread(&Music::streamData, this));
        }
        else
        {
            Sound::play();
        }
    }

    void Music::stop()
    {
        // if (status != Stopped)
        // {
        {
            std::lock_guard<std::mutex> lock(threadMutex);
            streaming = false;
        }
        streamThread->join();

        seek(0.0f);
        // Sound::stop();
        // clearQueue();

        samplesProcessed = 0;
        // }
    }

    void Music::seek(float time)
    {
        std::lock_guard<std::mutex> lock(threadMutex);

        if (file)
        {
            sf_count_t offset = static_cast<sf_count_t>(time * sampleRate);
            int code = sf_seek(file, offset, SEEK_SET);

            int error = sf_error(file);
            if (error == 0)
            {
                samplesProcessed = static_cast<unsigned int>(time * sampleRate * channelCount);
                Console::logf("seek %d", code);
            }
            else
            {
                Console::logf("libsndfile error(%d): %s", error, sf_strerror(file));
            }
        }
    }

    void Music::relSeek(float time)
    {
        float targetTime = getTime() + time;
        seek(targetTime);
    }

    float Music::getTime()
    {
        ALfloat seconds = 0.f;
        alGetSourcef(source, AL_SEC_OFFSET, &seconds);

        return seconds + static_cast<float>(samplesProcessed) / sampleRate / channelCount;
    }

    float Music::getDuration()
    {
        return duration;
    }

    void Music::setLoop(bool value)
    {
        loop = value;
    }

    bool Music::getLoop() const
    {
        return loop;
    }

    void Music::loadSound(const std::string& filename)
    {
        // Generate Buffers
        alGenBuffers(BUFFER_COUNT, buffers);

        for (auto& elem : finalBuffer)
            elem = false;
        __generateSource();

        SF_INFO FileInfos;
        file = sf_open(filename.c_str(), SFM_READ, &FileInfos);
        if (!file)
        {
            int error = sf_error(file);
            Console::logf("Failed to load \"%s\".", filename.c_str());
            Console::logf("libsndfile error(%d): %s", error, sf_strerror(file));

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
        std::lock_guard<std::mutex> lock(threadMutex);

        c.samples = &buffer[0];

        if (file != nullptr && c.samples && sampleCount)
            c.sampleCount = static_cast<std::size_t>(sf_read_short(file, &buffer[0], buffer.size()));
        else
            c.sampleCount = 0;

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
        SoundChunk chunk { nullptr, 0 };

        if (!loadChunk(chunk))
        {
            // Mark this buffer as the last buffer of the file.
            finalBuffer[index] = true;

            if (loop)
            {
                // Seek to the beginning of the song and load the final buffer.
                seek(0.0f);

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
        ALint queuedCount;
        alGetSourcei(source, AL_BUFFERS_QUEUED, &queuedCount);

        // Unqueue all the buffers
        ALuint buffer;
        for (ALint i = 0; i < queuedCount; ++i)
            alSourceUnqueueBuffers(source, 1, &buffer);
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

    void Music::streamData()
    {
        bool requestStop = fillQueue();
        play();

        while (!requestStop && streaming)
        {
            // Get the number of buffers that have already finished playing.
            ALint processedCount = buffersProcessed();

            while (processedCount--)
            {
                // Get an empty buffer
                ALuint buffer = popBuffer();
                unsigned int bufferNum = getBufferNum(buffer);

                // If this buffer is the final buffer for the file, reset the sample count
                // to prepare for possible music looping.
                if (finalBuffer[bufferNum])
                {
                    samplesProcessed = 0;
                    finalBuffer[bufferNum] = false;
                }
                else
                {
                    ALint size, bits;
                    alGetBufferi(buffer, AL_SIZE, &size);
                    alGetBufferi(buffer, AL_BITS, &bits);

                    samplesProcessed += (size / (bits / 8));
                }

                // Fill it and push it back into the playing queue
                if (!requestStop)
                {
                    if (fillBuffer(bufferNum))
                    {
                        requestStop = true;
                    }
                }
            }

            // Leave some time for the other threads if the stream is still playing
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        Sound::stop();

        // We can't have this thread call Music's stop method because it would cause a
        // deadlock (it would try to join itself).
        // stop();

        // Clear everything queued up to play.
        clearQueue();
    }
}
