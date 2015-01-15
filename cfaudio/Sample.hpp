#ifndef _Sample_hpp_
#define _Sample_hpp_

#include "Internal/Sound.hpp"
#include "Internal/SoundBuffer.hpp"

namespace cfAudio
{
    class Sample : public Sound
    {
        private:
            SoundBuffer buffer;

        public:
            Sample(const std::string& filename);
            virtual ~Sample() override;

            // Load a Sample from a file.
            // Uses libsndfile, table of supported filetypes can be found here:
            // http://www.mega-nerd.com/libsndfile/
            void loadSound(const std::string& filename) override;
    };
}

#endif // _Sample_hpp_
