#pragma once

#include "GameComponent.h"

namespace GlEngine::ShaderFactory
{
    class IPropertyProvider;
}

class WorldEditorObject;

class EditorControllerComponent : public GlEngine::GameComponent {
public:
    EditorControllerComponent(std::vector<GlEngine::ShaderFactory::IPropertyProvider*> providers, bool editing);
    ~EditorControllerComponent();

    virtual bool InitializeAsync() override;
    virtual void ShutdownAsync() override;

    virtual void HandleEvent(GlEngine::Events::Event &evt) override;

    bool ExecuteFile(std::string path);
    bool ExecuteStream(std::istream &stream);

    void SaveFile(std::string path);
    void SaveStream(std::ostream &stream);

    bool ExecuteCommand(std::string &command, std::string &line, std::istringstream &stream);

private:
    std::map<unsigned, bool> keysDown;
    bool editing;

    std::vector<GlEngine::ShaderFactory::IPropertyProvider*> _providers;
    std::vector<WorldEditorObject*> _objects;

    WorldEditorObject *_selected;
    WorldEditorObject *createObject(std::string &path);
    void deleteObject(WorldEditorObject *obj);
};