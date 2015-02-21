#ifndef _Sound_hpp_
#define _Sound_hpp_

#include <string>

#include <AL/al.h>
#include <AL/alc.h>

namespace cfAudio
{
    enum Status
    {
        Stopped, Playing, Paused
    };

    class Sound
    {
        protected:
            Status status;
            ALuint source;

            void __generateSource();
            void __setSource(const unsigned int& bufferID);

        public:
            Sound();
            Sound(const std::string& filename);
            virtual ~Sound();

            Sound(const Sound&) = delete;
            Sound& operator=(const Sound&) = delete;

            virtual void loadSound(const std::string& filename) = 0;

            virtual void play();
            virtual void stop();
            void pause();

            void setVolume(float volume);
            float getVolume();

            // Set the panning of the sound.
            // Range: [-1.0f, 1.0f]
            void setPan(float pan);
            float getPan();

            bool isPlaying();
            bool isPaused();
            bool isStopped();

            virtual void setLoop(bool loop);
            virtual bool getLoop() const;

            virtual void seek(float time) = 0;
            virtual float getTime() = 0;
            virtual float getDuration() = 0;

            ALuint getSource();
            Status getStatus();
    };
}

#endif // _Sound_hpp_
