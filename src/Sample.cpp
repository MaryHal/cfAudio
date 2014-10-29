#include <Sample.hpp>
#include <Internal/SoundLoader.hpp>

namespace cfAudio
{
    Sample::Sample(const std::string& filename)
        : Sound(filename)
    {
        loadSound(filename);
    }

    Sample::~Sample()
    {
    }

    void Sample::loadSound(const std::string& filename)
    {
        __generateSource();
        buffer.loadFromFile(filename);
        buffer.update();
        __setSource(buffer.getBufferID());
    }
}
