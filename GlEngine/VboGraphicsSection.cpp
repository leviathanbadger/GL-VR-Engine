#include "stdafx.h"
#include "VboGraphicsSection.h"

#include "VboFactory.h"
#include "Shader.h"
#include "Texture.h"
#include "OpenGl.h"

namespace GlEngine
{
    namespace Impl
    {
        VboGraphicsSection::VboGraphicsSection(Shader *shader, Texture *texture)
            : shader(shader),
              texture(texture),
              tris(new std::vector<Vector<3, uint16_t>>()),
              quads(new std::vector<Vector<4, uint16_t>>()),
              finalized(false)
        {
        }
        VboGraphicsSection::~VboGraphicsSection()
        {
            SafeDelete(tris);
            SafeDelete(quads);
        }

        void VboGraphicsSection::AddTriangle(Vector<3, uint16_t> indices)
        {
            assert(!finalized);
            tris->push_back(indices);
        }
        void VboGraphicsSection::AddQuad(Vector<4, uint16_t> indices)
        {
            assert(!finalized);
            quads->push_back(indices);
        }
        void VboGraphicsSection::Finalize(VboFactory<VboType::UnsignedShort, uint16_t>* face_factory)
        {
            assert(!finalized);

            finalized = true;
            triCount = tris->size();
            quadCount = quads->size();
            if (triCount > 0)
            {
                auto vec = tris->at(0);
                triOffset = OFFSET_PTR(uint16_t, face_factory->AddVertex(vec[0]));
                face_factory->AddVertex(vec[1]);
                face_factory->AddVertex(vec[2]);
                for (int q = 1; q < triCount; q++)
                {
                    vec = tris->at(q);
                    face_factory->AddVertex(vec[0]);
                    face_factory->AddVertex(vec[1]);
                    face_factory->AddVertex(vec[2]);
                }
            }
            if (quadCount > 0)
            {
                auto vec = quads->at(0);
                quadOffset = OFFSET_PTR(uint16_t, face_factory->AddVertex(vec[0]));
                face_factory->AddVertex(vec[1]);
                face_factory->AddVertex(vec[2]);
                face_factory->AddVertex(vec[3]);
                for (int q = 1; q < quadCount; q++)
                {
                    vec = quads->at(q);
                    face_factory->AddVertex(vec[0]);
                    face_factory->AddVertex(vec[1]);
                    face_factory->AddVertex(vec[2]);
                    face_factory->AddVertex(vec[3]);
                }
            }

            SafeDelete(tris);
            SafeDelete(quads);
        }

        void VboGraphicsSection::PreRender()
        {
            if (shader != nullptr && *shader) shader->MakeCurrent();
            if (texture != nullptr && *texture) texture->MakeCurrent();
        }
        void VboGraphicsSection::Render()
        {
            GraphicsObject::Render();
            if (*this)
            {
                if (triCount) glDrawElements(GL_TRIANGLES, triCount * 3, static_cast<GLenum>(VboType::UnsignedShort), triOffset);
                if (quadCount) glDrawElements(GL_QUADS, quadCount * 4, static_cast<GLenum>(VboType::UnsignedShort), quadOffset);
            }
        }

        const char *VboGraphicsSection::name()
        {
            return "VboGraphicsSection";
        }
        VboGraphicsSection::operator bool()
        {
            return finalized && *shader && *texture;
        }
    }
}