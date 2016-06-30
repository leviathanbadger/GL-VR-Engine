#pragma once

#include "OpenGl.h"
#include "VbObject.h"

#include "IComponent.h"
#include "Shader.h"

namespace GlEngine
{
	namespace Impl
	{
		class ENGINE_SHARED GraphicsObjectImpl : public IComponent
		{
		public:
			GraphicsObjectImpl();
			~GraphicsObjectImpl();

            bool Initialize();
            void Shutdown();

			VbObject arrayVbo;
            VbObject elementVbo;
            Shader shader;

			void Render();
		};
	}
}
