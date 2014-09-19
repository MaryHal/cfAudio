#ifndef _Sound_hpp_
#define _Sound_hpp_

#include <string>

#include <AL/al.h>
#include <AL/alc.h>

enum Status { Stopped, Playing, Paused };

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
        ~Sound();

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

        virtual void seek(float time);
        virtual float getTime();
        virtual float getDuration();

        ALuint getSource();
        Status getStatus();
};

#endif // _Sound_hpp_
