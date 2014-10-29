#include <Internal/SoundLoader.hpp>

namespace cfAudio
{
    ALenum SoundLoader::getFormat(unsigned int channels)
    {
        switch (channels)
        {
        case 1 : return AL_FORMAT_MONO16;
        case 2 : return AL_FORMAT_STEREO16;
        case 4 : return alGetEnumValue("AL_FORMAT_QUAD16");
        case 6 : return alGetEnumValue("AL_FORMAT_51CHN16");
        case 7 : return alGetEnumValue("AL_FORMAT_61CHN16");
        case 8 : return alGetEnumValue("AL_FORMAT_71CHN16");
        }

        return 0;
    }
}
