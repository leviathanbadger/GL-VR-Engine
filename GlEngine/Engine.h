#pragma once

#include "IComponent.h"
#include "EventQueue.h"
#include "ServiceProvider.h"
#include "Window.h"

#define THIS_ENGINE (GlEngine::Engine::GetInstance())

namespace GlEngine
{
    class WindowManager;
    class GraphicsController;
    class AudioController;

    class ENGINE_SHARED Engine : public IComponent
    {
    private:
        Engine();
        ~Engine();

        static Engine _instance;

    public:
        static constexpr Engine &GetInstance()
        {
            return _instance;
        }

        bool Initialize() override;
        void Shutdown() override;

        WindowManager &GetWindowManager();
        GraphicsController &GetGlController();
        AudioController &GetAudioController();
        inline Events::EventQueue &GetEventQueue()
        {
            return _events;
        }
        inline ServiceProvider &GetServiceProvider()
        {
            return _services;
        }

        rt_mutex &GetMutex();

        virtual std::string name() override;

        void MessageLoop();

    private:
        Events::EventQueue _events;
        ServiceProvider _services;
    };
}
