#include "stdafx.h"
#include "TileRPGGame.h"

#include "Engine.h"
#include "WindowManager.h"
#include "GraphicsContext.h"
#include "TransformedGraphicsObject.h"
#include "Camera.h"

#include "GlRenderTarget.h"
#include "OpenGl.h"

#include "TestGameObject.h"

#include "FileLogger.h"
#include "FileConfigProvider.h"

namespace TileRPG
{
    TileRPGGame::TileRPGGame()
    {
    }
    TileRPGGame::~TileRPGGame()
    {
        Shutdown();
    }

    bool TileRPGGame::createWindow()
    {
        auto &engine = GlEngine::Engine::GetInstance();
        if (!engine.Initialize()) return false;

        _window = engine.GetWindowManager().Create();
        if (_window == nullptr)
        {
            engine.Shutdown();
            return false;
        }

        if (config->GetValueWithDefault("Fullscreen", true)) _window->SetFullscreen(true);
        _renderTarget = new GlEngine::GlRenderTarget(_window);
        if (!_renderTarget->Initialize())
        {
            engine.Shutdown(); //This will call _window.Shutdown(), we don't have to do it
            return false;
        }

        _gfxContext = new GlEngine::GraphicsContext();
        _gfxContext->AddRenderTarget(_renderTarget);

        _gfxContext->camera.SetEye({ 0, 0, 0 });
        _gfxContext->camera.SetTarget({ 0, 0, 1 });
        _gfxContext->camera.SetUp({ 0, 1, 0 });

        if (!_gfxContext->Initialize())
        {
            _renderTarget->Shutdown();
            engine.Shutdown(); //This will call _window.Shutdown(), we don't have to do it
            return false;
        }

        _window->Show();
		
        return true;
    }
    void TileRPGGame::destroyWindow()
    {
        if (_gfxContext != nullptr)
        {
            _gfxContext->Shutdown();
            delete _gfxContext;
            _gfxContext = nullptr;
        }

        if (_renderTarget != nullptr)
        {
            _renderTarget->Shutdown();
            delete _renderTarget;
            _renderTarget = nullptr;
        }

        auto &engine = GlEngine::Engine::GetInstance();
        engine.Shutdown();
    }

    bool TileRPGGame::Initialize()
    {
        logger = registerLogger();
        if (logger == nullptr) return false;
        config = registerConfig();
        if (config == nullptr) return false;

        logger->Log(GlEngine::LogType::InfoC, "Welcome to TileRPG! Beginning game initialization...");

        if (!_loop.Initialize())
        {
            logger->Log(GlEngine::LogType::FatalErrorC, "Could not initialize TileRPG. Aborting...");
            Shutdown();
            return false;
        }
        if (!createWindow())
        {
            logger->Log(GlEngine::LogType::FatalErrorC, "Could not initialize TileRPG. Aborting...");
            Shutdown();
            return false;
        }

        auto gameObject = new TestGameObject();
        auto graphicsObject = new GlEngine::GraphicsObject();
        _gfxContext->Register(gameObject, graphicsObject);
        _loop.GetGameLogic().AddGameObject(gameObject);

        //_gfxContext->Update();
        //_gfxContext->Render();

        logger->Log(GlEngine::LogType::InfoC, "TileRPG initialization successful. Beginning game.");
        return true;
    }
    void TileRPGGame::Shutdown()
    {
        logger->Log(GlEngine::LogType::Info, "~Shutting down TileRPGGame...");

        destroyWindow();
        _loop.Shutdown();
    }

    void TileRPGGame::MessageLoop()
    {
        logger->Log(GlEngine::LogType::Info, "Beginning TileRPGGame MessageLoop...");

        auto &engine = GlEngine::Engine::GetInstance();
        engine.MessageLoop();
    }

    GlEngine::ILogger *TileRPGGame::registerLogger()
    {
        auto f_logger = new GlEngine::FileLogger("", "TileRPG.log");
        if (!f_logger->Initialize())
        {
            f_logger->Log(GlEngine::LogType::FatalErrorC, "Could not initialize file logger. Aborting");
            return nullptr;
        }

        auto &serviceProvider = GlEngine::Engine::GetInstance().GetServiceProvider();
        serviceProvider.RegisterService<GlEngine::ILogger>(f_logger);
        return f_logger;
    }
    GlEngine::IConfigProvider *TileRPGGame::registerConfig()
    {
        auto f_config = new GlEngine::FileConfigProvider("", "TileRPG.config");
        if (!f_config->Initialize())
        {
            logger->Log(GlEngine::LogType::FatalErrorC, "Could not initialize file config provider. Aborting");
            return nullptr;
        }

        auto &serviceProvider = GlEngine::Engine::GetInstance().GetServiceProvider();
        serviceProvider.RegisterService<GlEngine::IConfigProvider>(f_config);
        return f_config;
    }
}
