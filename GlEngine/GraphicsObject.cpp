#include "stdafx.h"
#include "GraphicsObject.h"

#include "OpenGl.h"
#include "VBOFactory.h"

#include "Engine.h"
#include "ServiceProvider.h"
#include "ILogger.h"
#include "ResourceLoader.h"

namespace GlEngine
{
	GraphicsObject::GraphicsObject()
	{
        initialized = false;
        graphicsInitialized = false;

        auto resources = Engine::GetInstance().GetServiceProvider().GetService<ResourceLoader>();
        resources->QueueResource(this);
	}
	GraphicsObject::~GraphicsObject()
	{
	}

	bool GraphicsObject::Initialize()
	{
        if (initialized) return true;
		return initialized = true;
	}
    void GraphicsObject::Shutdown()
    {
        initialized = false;
	}
    bool GraphicsObject::InitializeGraphics()
    {
        if (graphicsInitialized) return true;
        if (!initialized)
        {
            auto logger = Engine::GetInstance().GetServiceProvider().GetService<ILogger>();
            logger->Log(LogType::ErrorC, "GraphicsObject graphics initialized before the object could be initialized. %s", this);
            return false;
        }
        return graphicsInitialized = true;
    }
    void GraphicsObject::ShutdownGraphics()
    {
        graphicsInitialized = false;
    }

	void GraphicsObject::PreRender()
	{
	}
	void GraphicsObject::Render()
	{
		PreRender();
	}
}
