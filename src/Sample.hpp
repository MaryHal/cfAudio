#ifndef _Sample_hpp_
#define _Sample_hpp_

#include "Internal/Sound.hpp"
#include "Internal/SoundBuffer.hpp"

class Sample : public Sound
{
private:
    SoundBuffer buffer;

public:
    Sample(const std::string& filename);
    ~Sample();

    void loadSound(const std::string& filename);
};

#endif // _Sample_hpp_
