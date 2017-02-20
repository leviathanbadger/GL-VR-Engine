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
    GraphicsObject::GraphicsObject(bool autoinit)
        : initialized(false),
          graphicsInitialized(false),
          renderOrder(0)
    {
        if (autoinit)
        {
            auto resources = Engine::GetInstance().GetServiceProvider().GetService<ResourceLoader>();
            resources->QueueInitialize(this);
        }
    }
    GraphicsObject::~GraphicsObject()
    {
    }

    bool GraphicsObject::InitializeAsync()
    {
        if (initialized) return true;
        return initialized = true;
    }
    void GraphicsObject::ShutdownAsync()
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

    void GraphicsObject::AddPropertyProvider(ShaderFactory::IPropertyProvider *provider)
    {
        _providers.push_back(provider);
    }
    void GraphicsObject::RemovePropertyProvider(ShaderFactory::IPropertyProvider *provider)
    {
        auto it = std::find(_providers.begin(), _providers.end(), provider);
        if (it != _providers.end()) _providers.erase(it);
    }

    void GraphicsObject::Render(RenderTargetLayer layer)
    {
        PreRender(layer);
        RenderImpl(layer);
        PostRender(layer);
    }
    void GraphicsObject::PreRender(RenderTargetLayer)
    {
    }
    void GraphicsObject::PostRender(RenderTargetLayer)
    {
    }

    void GraphicsObject::RenderInstanced(RenderTargetLayer layer, unsigned instanceCount)
    {
        if (instanceCount == 0) return;
        PreRenderInstanced(layer);
        RenderInstancedImpl(layer, instanceCount);
        PostRenderInstanced(layer);
    }
    void GraphicsObject::PreRenderInstanced(RenderTargetLayer)
    {
    }
    void GraphicsObject::PostRenderInstanced(RenderTargetLayer)
    {
    }

    void GraphicsObject::SetInstanceCount(unsigned count)
    {
        instanceCount = count;
    }

    std::vector<ShaderFactory::IPropertyProvider*> &GraphicsObject::providers()
    {
        return _providers;
    }
}
