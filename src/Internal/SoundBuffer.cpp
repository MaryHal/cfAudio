#include "SoundBuffer.hpp"
#include "SoundLoader.hpp"

#include <AL/al.h>
#include <sndfile.h>

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
        //std::cerr << "Failed to read sound file \"" << Filename << "\"" << std::endl;
        return;
    }

    // Set the sound parameters
    sampleCount = static_cast<std::size_t>(FileInfos.frames) * FileInfos.channels;
    channelCount = FileInfos.channels;
    sampleRate = FileInfos.samplerate;

    // Read Data
    allocateData(sampleCount * sizeof(short));
    static_cast<std::size_t>(sf_read_short(myFile, (short*)data, sampleCount));

    sf_close(myFile);

    // We now need to generate a buffer, fill it with data, then set our
    // source to play the buffer.
    alGenBuffers(1, &buffer);
}

void SoundBuffer::update()
{
    // Find the good format according to the number of channels
    ALenum format = SoundLoader::getFormat(channelCount);

    // Check if the format is valid
    if (format == 0)
    {
        //std::cerr << "Unsupported number of channels (" << ChannelsCount << ")" << std::endl;
        return;
    }

    // Fill the buffer
    ALsizei size = static_cast<ALsizei>(sampleCount) * sizeof(short);
    alBufferData(buffer, format, data, size, sampleRate);

    // Compute the duration
    // myDuration = static_cast<float>(mySamples.size()) / SampleRate / ChannelsCount;
}

const unsigned int SoundBuffer::getBufferID() const
{
    return buffer;
}

const std::size_t SoundBuffer::getSampleCount() const
{
    return sampleCount;
}

