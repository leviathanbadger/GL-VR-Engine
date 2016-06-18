#pragma once

#include "EngineShared.h"

namespace GlEngine
{
	namespace Impl
	{
		class GraphicsObjectImpl;
	}

	class ENGINE_SHARED GraphicsObject
	{
	public:
		GraphicsObject();
		~GraphicsObject();

		void Render();

	private:
		Impl::GraphicsObjectImpl * pimpl;
	};
}