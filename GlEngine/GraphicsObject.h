#pragma once

#include "IGraphicsComponent.h"

namespace GlEngine
{
	namespace Impl
	{
		class GraphicsObjectImpl;
	}

	class ENGINE_SHARED GraphicsObject : public IGraphicsComponent
	{
	public:
		GraphicsObject();
		~GraphicsObject();

        bool Initialize() override;
        void Shutdown() override;
        bool InitializeGraphics() override;
        void ShutdownGraphics() override;
		
		virtual void PreRender();
		virtual void Render();

        virtual operator bool() = 0;

    protected:
        bool initialized, graphicsInitialized;
	};
}
