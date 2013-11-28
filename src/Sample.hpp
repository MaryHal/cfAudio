#ifndef _Sample_hpp_
#define _Sample_hpp_

#include "Sound.hpp"

class Sample : public Sound
{
    private:
        SoundBuffer buffer;

    public:
        Sample();
        Sample(const std::string& filename);
        ~Sample();

        void loadSound(const std::string& filename);
};

#endif

