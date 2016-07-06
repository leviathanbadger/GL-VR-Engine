#pragma once

#include "IAudioSource.h"
#include "Audio.h"

namespace GlEngine
{
    class YseAudioSource : public IAudioSource
    {
    public:
        YseAudioSource();
        ~YseAudioSource();

        void SetSource(const char *const filename) override;
        void SetPosition(Vector<3> &position) override;
        void SetSpeed(Vector<3> &speed) override;
        bool Play(bool loop = false) override;
        void Stop(unsigned millis = 0) override;

        void Update() override;

    private:
        YSE::sound sound;
        bool isPlaying, soundStarted;
    };
}
