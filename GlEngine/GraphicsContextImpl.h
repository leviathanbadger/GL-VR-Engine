#pragma once

#include "ScopedLock.h"
#include <set>
#include <mutex>
#include "TransformedGraphicsObject.h"

namespace GlEngine
{
	namespace Impl
	{
		class GraphicsContextImpl
		{
		public:
			GraphicsContextImpl();
			~GraphicsContextImpl();

			std::mutex _lock;
			std::set<TransformedGraphicsObject*> objects;

			bool Initialize();
			void Shutdown();

			void AddGraphicsObject(TransformedGraphicsObject * graphicsObject);
			void RemoveGraphicsObject(TransformedGraphicsObject * graphicsObject);

			void Render();
		};
	}
}
