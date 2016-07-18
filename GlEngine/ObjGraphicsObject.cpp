#include "stdafx.h"
#include "ObjGraphicsObject.h"

#include "ObjLoader.h"

#include "Shader.h"
#include "Texture.h"
#include <unordered_map>

namespace GlEngine
{
	ObjGraphicsObject::ObjGraphicsObject(const char *const filename)
        : filename(filename)
	{
	}
    ObjGraphicsObject::ObjGraphicsObject(const char *const filename, VbObject arrayVbo, VbObject elementVbo)
        : VboGraphicsObject(arrayVbo, elementVbo), filename(filename)
    {
    }
    ObjGraphicsObject::ObjGraphicsObject(const char *const filename, VbObject arrayVbo, VbObject elementVbo, Shader *shader, Texture *texture)
        : VboGraphicsObject(arrayVbo, elementVbo, shader, texture), filename(filename)
    {
    }

    ObjGraphicsObject *ObjGraphicsObject::Create(const char *name, const char *shader_path, const char *shader_name, const char *texture_filename)
    {
        auto hashed = ([](const char *str) {
            int h = 0;
            while (*str)
                h = h << 1 ^ *str++;
            return h;
        })(name);

        Shader *shader = Shader::Create(shader_path, shader_name);
        Texture *texture = texture_filename == nullptr || texture_filename[0] == '\0' ? nullptr :
                           Texture::FromFile(texture_filename);

        static std::unordered_map<int, ObjGraphicsObject*> cache;
        auto ptr = cache[hashed];
        if (ptr == nullptr) ptr = cache[hashed] = new ObjGraphicsObject(name, VbObject(), VbObject(), shader, texture);
        return ptr;
    }

	bool ObjGraphicsObject::Initialize()
	{
		if (!GlEngine::ObjLoader::Load(filename, this))
			return false;
		return VboGraphicsObject::Initialize();
	}

    const char *ObjGraphicsObject::name()
    {
        return "ObjGraphicsObject";
    }
}
