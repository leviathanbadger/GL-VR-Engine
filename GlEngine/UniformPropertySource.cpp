#include "stdafx.h"
#include "UniformPropertySource.h"
#include "Program.h"
#include "Snippet.h"

namespace GlEngine
{
    namespace ShaderFactory
    {
        UniformPropertySource::UniformPropertySource(std::vector<ShaderProp*> properties)
        {
            assert(properties.size() >= 1);
            for (size_t q = 0; q < properties.size(); q++)
            {
                assert(properties[q] != nullptr);
                _props.push_back(properties[q]);
            }
        }
        UniformPropertySource::~UniformPropertySource()
        {
        }

        bool UniformPropertySource::HasProperty(ShaderProp *prop)
        {
            return std::find(_props.begin(), _props.end(), prop) != _props.end();
        }

        const std::vector<ShaderProp*> UniformPropertySource::outProperties()
        {
            return properties;
        }

        void UniformPropertySource::ProvideProperty(ShaderProp *prop, Program *program, ComponentType type)
        {
            assert(HasProperty(prop));
            unsigned idx = program->FindOrCreateUniform(prop);
            program->components[type]->uniforms[idx] = prop;
            program->components[type]->snippets.insert(program->components[type]->snippets.begin(), Snippet::IdentitySnippet(prop, true, false));
        }
    }
}
