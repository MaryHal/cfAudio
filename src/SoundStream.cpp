#include "SoundStream.hpp"
#include "Music.hpp"

#include "System/Log.hpp"

#include <AL/al.h>

void SoundStream::streamData(void* arg)
{
    Music* m = (Music*)arg;

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
        // glfwSleep(0.2);
    }

    // Stop the playback
    // m->stop();

    // Unqueue any buffer left in the queue
    m->clearQueue();

    log("Music Stream Thread Killed");
}
