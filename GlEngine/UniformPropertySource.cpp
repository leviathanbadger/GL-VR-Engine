#include "stdafx.h"
#include "UniformPropertySource.h"
#include "Program.h"
#include "Snippet.h"

GlEngine::ShaderFactory::UniformPropertySource::UniformPropertySource(ShaderProp * prop)
    : prop(prop)
{
}

bool GlEngine::ShaderFactory::UniformPropertySource::HasProperty(ShaderProp * prop)
{
    return this->prop == prop;
}

void GlEngine::ShaderFactory::UniformPropertySource::ProvideProperty(ShaderProp * prop, Program * program, ComponentType type)
{
    unsigned idx = program->FindOrCreateUniform(prop);
    program->components[type]->uniforms[idx] = prop;
    program->components[type]->orderedSnippets.insert(program->components[type]->orderedSnippets.begin(), Snippet::IdentitySnippet(prop, true, false));
}
