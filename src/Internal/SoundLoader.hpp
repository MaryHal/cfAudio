#ifndef _SoundLoader_hpp_
#define _SoundLoader_hpp_

#include <string>
#include <AL/al.h>

#include "SoundBuffer.hpp"

class SoundLoader
{
public:
    static ALenum getFormat(unsigned int channels);
};

#endif
