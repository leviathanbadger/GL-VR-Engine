#pragma once

#include "IGraphicsComponent.h"
#include "TesselationType.h"
#include "RenderTargetLayer.h"

namespace GlEngine
{
    class Texture;

    class ENGINE_SHARED Material : public IGraphicsComponent
    {
    public:
        Material(bool is2d = false);
        ~Material();

        bool Initialize() override;
        void Shutdown() override;
        bool InitializeGraphics() override;
        void ShutdownGraphics() override;

        virtual void Push(bool instanced = false) = 0;
        virtual void Pop(bool instanced = false) = 0;

        virtual bool IsOpaque() = 0;

        inline bool Is2d()
        {
            return is2d;
        }

        virtual RenderTargetLayer GetRenderTargetLayer();

        virtual TesselationType GetTesselationType() = 0;

    private:
        bool is2d;
    };
}
