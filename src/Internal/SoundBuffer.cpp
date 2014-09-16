#include <Internal/SoundBuffer.hpp>
#include <Internal/SoundLoader.hpp>

#include <sndfile.h>

#include "../System/Log.hpp"

SoundBuffer::SoundBuffer()
{
}

SoundBuffer::SoundBuffer(const std::string& filename)
{
    loadFromFile(filename);
}

SoundBuffer::~SoundBuffer()
{
    alDeleteBuffers(1, &buffer);
}

void SoundBuffer::loadFromFile(const std::string& filename)
{
    SF_INFO FileInfos;
    SNDFILE* myFile = sf_open(filename.c_str(), SFM_READ, &FileInfos);
    if (!myFile)
    {
        Console::logf("Failed to load \"%s\".", filename.c_str());
        return;
    }

    // Set the sound parameters
    sampleCount = static_cast<std::size_t>(FileInfos.frames) * FileInfos.channels;
    channelCount = FileInfos.channels;
    sampleRate = FileInfos.samplerate;

    // Read Data
    allocateData(sampleCount * sizeof(short));
    if (static_cast<std::size_t>(sf_read_short(myFile, (short*)data, sampleCount)) == sampleCount)
    {
        update();
    }

    sf_close(myFile);

    // We now need to generate a buffer, fill it with data, then set our
    // source to play the buffer.
    alGenBuffers(1, &buffer);
}

void SoundBuffer::update()
{
    // Find a good format according to the number of channels
    ALenum format = SoundLoader::getFormat(channelCount);

    // Check if the format is valid
    if (format == 0)
    {
        Console::logf("Unsupported number of channels (%d).", channelCount);
        return;
    }

    // Fill the buffer
    ALsizei size = static_cast<ALsizei>(sampleCount) * sizeof(short);
    alBufferData(buffer, format, data, size, sampleRate);

    // Compute the duration
    // duration = static_cast<float>(sampleCount) / sampleRate / channelCount;
}

unsigned int SoundBuffer::getBufferID() const
{
    return buffer;
}

std::size_t SoundBuffer::getSampleCount() const
{
    return sampleCount;
}
