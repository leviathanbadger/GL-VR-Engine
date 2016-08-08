#pragma once

#include "RenderTargetImpl.h"

namespace GlEngine
{
	namespace Impl
	{
		class ENGINE_SHARED WindowRenderTargetImpl : RenderTargetImpl
		{
		public:
			WindowRenderTargetImpl(Window * window);
			~WindowRenderTargetImpl();

			virtual bool Initialize() override;
			virtual void Shutdown() override;

			virtual const char *name() override;

			bool alive = true;

			virtual void MakeCurrentTarget() override;

			virtual void Prepare() override;
			virtual void Push() override;
			virtual void Pop() override;

			virtual void Flip() override;

			int depthBufferBits = 24;
			int stencilBufferBits = 8;
			int auxFrameBufferCount = 0;
			int frameBufferColorBits = 32;
			int frameBufferType = PFD_TYPE_RGBA;

			int pixelFormatAdditionalFlags = 0;

		private:
			Window *_window;
			HDC deviceContext;
			HGLRC contextHandle;
			unsigned lastWidth, lastHeight;
			bool shouldRender;

			bool CreateContext();
			bool LoadGlewExtensions();
		};
	}
}