#include "stdafx.h"
#include "RenderTargetImpl.h"
#include "MatrixStack.h"
#include <chrono>

namespace GlEngine
{
    namespace Impl
    {
        RenderTargetImpl::RenderTargetImpl()
        {
            for (int i = 0; i < layerCount; i++)
                viewPorts[i] = nullptr;
        }
        RenderTargetImpl::~RenderTargetImpl()
        {
        }

        bool RenderTargetImpl::Initialize()
        {
            return true;
        }
        void RenderTargetImpl::Shutdown()
        {
            for (int i = 0; i < layerCount; i++)
            {
                if (viewPorts[i] != nullptr)
                    delete viewPorts[i];
            }
        }
        bool RenderTargetImpl::InitializeGraphics()
        {
            return true;
        }
        void RenderTargetImpl::ShutdownGraphics()
        {
        }

        const char *RenderTargetImpl::name()
        {
            return "RenderTargetImpl";
        }

        void RenderTargetImpl::MakeCurrentTarget()
        {
        }

        void RenderTargetImpl::Prepare()
        {
        }
        void RenderTargetImpl::PrePush()
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        void RenderTargetImpl::Push(RenderTargetLayer layer, RenderTargetViewMode viewMode, Matrix<4, 4> viewMatrix)
        {
            glEnable(GL_TEXTURE_2D);
            if (layer == RenderTargetLayer::Layer3dOpaque || layer == RenderTargetLayer::Layer3dTransluscent)
            {
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
                glDepthFunc(GL_LEQUAL);
            }
            else if (layer == RenderTargetLayer::Layer2d)
            {
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                glDepthFunc(GL_NONE);
                MatrixStack::Model.push(Matrix<4, 4>::Identity());
                MatrixStack::View.push(Matrix<4, 4>::Identity());
            }

            ViewPort* viewPort = this->viewPorts[(int)layer - (int)std::numeric_limits<RenderTargetLayer>::min()];
            if (viewPort != nullptr) viewPort->Push();

            if (viewMode == RenderTargetViewMode::Relative) MatrixStack::View.mult(viewMatrix);
            else if (viewMode == RenderTargetViewMode::Absolute) MatrixStack::View.push(viewMatrix);
            else assert(false);
        }
        void RenderTargetImpl::Pop(RenderTargetLayer layer)
        {
            if (layer == RenderTargetLayer::Layer2d)
            {
                MatrixStack::Model.pop();
                MatrixStack::View.pop();
            }

            ViewPort* viewPort = this->viewPorts[(int)layer - (int)std::numeric_limits<RenderTargetLayer>::min()];
            if (viewPort != nullptr) viewPort->Pop();

            MatrixStack::View.pop();
        }

        void RenderTargetImpl::Flip()
        {
        }

        void RenderTargetImpl::SetViewPort(RenderTargetLayer layer, ViewPort *viewPort)
        {
            auto idx = (int)layer - (int)std::numeric_limits<RenderTargetLayer>::min();
            const unsigned count = (int)std::numeric_limits<RenderTargetLayer>::max() + 1 - (int)std::numeric_limits<RenderTargetLayer>::min();
            if (idx < 0 || idx >= count) return;

            ViewPort* mViewPort = this->viewPorts[idx];
            if (mViewPort != nullptr)
                delete mViewPort;
            this->viewPorts[(int)layer - (int)std::numeric_limits<RenderTargetLayer>::min()] = viewPort;
        }
        ViewPort *RenderTargetImpl::viewPort(RenderTargetLayer layer)
        {
            auto idx = (int)layer - (int)std::numeric_limits<RenderTargetLayer>::min();
            const unsigned count = (int)std::numeric_limits<RenderTargetLayer>::max() + 1 - (int)std::numeric_limits<RenderTargetLayer>::min();
            if (idx < 0 || idx >= count) return nullptr;

            return this->viewPorts[idx];
        }
    }
}
