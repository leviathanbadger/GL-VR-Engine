#pragma once

#include "PhongMaterial.h"
#include "Attribute.h"

extern GlEngine::ShaderFactory::Attribute attr_PhongFog;

class PhongFogMaterial : public GlEngine::PhongMaterial
{
public:
    PhongFogMaterial(Vector<3> color, Vector<3> reflectionCoef = { 1.f, 1.f, 1.f }, float shininess = 8);
    PhongFogMaterial(GlEngine::Texture *texture, Vector<3> reflectionCoef = { 1.f, 1.f, 1.f }, float shininess = 8);
    ~PhongFogMaterial();

public:
    virtual std::vector<GlEngine::ShaderFactory::Attribute*> attributes() override;

    const char *name() override;
};
