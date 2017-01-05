#pragma once

#include "Property.h"
#include "Snippet.h"
#include "ComponentType.h"
#include <set>

namespace GlEngine
{
    namespace ShaderFactory
    {
        extern std::string ComponentTypeName(ComponentType type)
        {
            switch (type)
            {
            case ComponentType::Vertex:
                return "Vertex";
            case ComponentType::TessControl:
                return "TessControl";
            case ComponentType::TessEvaluation:
                return "TessEvaluation";
            case ComponentType::Geometry:
                return "Geometry";
            case ComponentType::Fragment:
                return "Fragment";
            }
            return "";
        }

        class ENGINE_SHARED Component
        {
        public:
            Component(ComponentType type);
            ~Component();

            ComponentType type;

            std::set<Property*> constants;
            std::set<Property*> uniforms;
            std::set<Property*> ins;
            std::set<Property*> outs;

            std::set<Snippet*> snippets;

            virtual std::string Compile();

        private:
            std::string CompileVersion();
            std::string CompileLayouts();
			std::string CompileBody();
            std::string CompileSource();

            Snippet* constantsSnippet;
            void CreateConstantsSnippet();

			void ResolveProperties();
            std::set<Property*> localProperties;
            std::vector<Snippet*> orderedSnippets;
            bool ResolveSnippetOrder();
            bool SnippetDependenciesMet(Snippet* snippet);
        };

        //TODO iterator
        template<typename T>
        class ComponentArray
        {
        public:
            ComponentArray(unsigned length)
            {
                _components = new T[length];
                size = length;
            }
            ComponentArray(std::vector<T> data)
            {
                _components = new T[data.size()];
                size = data.size();
                for (unsigned i = 0; i < data.size(); i++)
                    _components[i] = data[i];
            }
            ~ComponentArray()
            {
                delete[] _components;
            }

            T& operator[](int index)
            {
                return _components[index];
            }
            T& operator[](ComponentType index)
            {
                return _components[(unsigned)index];
            }
            size_t size()
            {
                return size;
            }

        private:
            size_t size;
            T* _components;
        };
    }
}

GlEngine::ShaderFactory::ComponentType std::numeric_limits<GlEngine::ShaderFactory::ComponentType>::min()
{
    return GlEngine::ShaderFactory::ComponentType::Vertex;
}
GlEngine::ShaderFactory::ComponentType std::numeric_limits<GlEngine::ShaderFactory::ComponentType>::max()
{
    return GlEngine::ShaderFactory::ComponentType::Fragment;
}