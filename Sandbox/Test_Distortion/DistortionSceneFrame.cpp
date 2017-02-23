#include "stdafx.h"
#include "DistortionSceneFrame.h"

#include "CameraComponent.h"
#include "../CameraTargetComponent.h"
#include "../LabControlsComponent.h"

#include "../RawGraphicsObject.h"
#include "../TemplateObj.h"
#include "../TemplateMaterial.h"
#include "../TemplateMaterialFactory.h"

#include "TextureRenderTarget.h"

#include "Engine.h"
#include "KeyboardEvent.h"
#include "GraphicsController.h"
#include "Property.h"
#include "../Lab05_Geometry/GeometrySceneFrame.h"
#include "../Lab04_Textures/TexturesSceneFrame.h"
#include "../Lab03_LightsAndEffects/LightsAndEffectsSceneFrame.h"

//HACK HACK HACK
#include "../Sandbox.h"
#include "RenderTarget.h"

extern std::string distortVertex;
extern std::string distortFragment;
static GlEngine::ShaderFactory::Property<GlEngine::ShaderFactory::Subroutine*> prop_Subroutine("__subroutine__");
static GlEngine::ShaderFactory::Property<GlEngine::ShaderFactory::Array<float, 25>> prop_ConvolutionKernel("convolution_kernel");

DistortionSceneFrame::DistortionSceneFrame()
    : pair_index(0)
{
    props = {
        { 0, &GlEngine::ShaderFactory::prop_GameTime },
        { 1, &GlEngine::ShaderFactory::prop_ScreenDimensions },
        { 2, &GlEngine::ShaderFactory::prop_Texture },
        { 3, &prop_ConvolutionKernel },
        { 4, &prop_Subroutine }
    };

    RefreshPair();

    distortSource = {
        &distortVertex,
        nullptr,
        nullptr,
        nullptr,
        &distortFragment,
    };
}
DistortionSceneFrame::~DistortionSceneFrame()
{
    SafeDelete(renderedFrame);
    SafeDelete(sceneTex);
}

bool DistortionSceneFrame::Initialize()
{
    if (!Frame::Initialize()) return false;

    this->renderedFrame = new GeometrySceneFrame();
    this->renderedFrame->Initialize();

    //auto ambient = new GlEngine::AmbientLightSource({ .25f, .25f, .25f });
    //auto pointLight = PointLightSourceObject::Create(this, "PointLight1");
    //auto lightSource = pointLight->component<PointLightSourceObject>()->lightSource();
    //controlsComponent->SetControllingLight(lightSource);
    //lightSource->SetPosition({ 0, 2.5, -2.5 });

    //auto appleTex = GlEngine::Texture::FromFile("Textures/apple.png");
    //auto cube1 = CreateGameObject<GlEngine::CubeGraphicsObject>(Vector<3> { 3, 3, 3 }, appleTex);
    //cube1->graphicsObject()->AddPropertyProvider(ambient);
    //cube1->graphicsObject()->AddPropertyProvider(pointLight->lightSource());
    //cube1->SetPosition({ -14.f, 0, 0 });
    ////cube1->SetRotationSpeed({ 0, 45deg, 0 });

    sceneTex = new GlEngine::TextureRenderTarget(1920, 1080 - 60, GlEngine::TextureFlag::Clamp);
    sceneTex->SetCamera(this->renderedFrame->findChild("Camera")->component<GlEngine::CameraComponent>());
    sceneTex->AddToGraphicsLoop();

    auto cameraTarget = CameraTargetComponent::Create(this, "CameraTarget");

    auto cameraObject = GlEngine::CameraComponent::Create(this, "Camera");
    cameraObject->transform.position = { 0, -3.5, 7 };

    auto cameraComponent = cameraObject->component<GlEngine::CameraComponent>();
    cameraComponent->SetTargetObject(cameraTarget);
    cameraComponent->SetLock(GlEngine::CameraLock::RELATIVE_POSITION);
    Sandbox::windowRenderTarget->SetCamera(cameraComponent);

    auto controls = new GlEngine::GameObject(this, "LabControlsComponent");
    auto controlsComponent = new LabControlsComponent();
    controls->AddComponent(controlsComponent);

    auto clipPlaneGobj = new GlEngine::GameObject(this, "ClipPlane");
    clipPlaneGobj->AddComponent(new TemplateObj(
        new RawGraphicsObject(
            "ClipPlaneGfx",
            "Resources/clip_plane.obj",
            &this->distortSource,
            &this->props
        ),
        TemplateMaterial::Factory()
            ->Name("DistortMaterial")
            ->ProvideConst(&GlEngine::ShaderFactory::prop_Texture, sceneTex)
            ->ProvideArray(&prop_ConvolutionKernel, currentConvolutionKernel)
            ->Provide(&prop_Subroutine, currentSubroutine)
            ->Create()
    ));

    return renderedFrame->Initialize();
}
void DistortionSceneFrame::Tick(float delta)
{
    if (renderedFrame == nullptr) return;
    renderedFrame->Tick(delta);
}
void DistortionSceneFrame::Shutdown()
{
    if (renderedFrame == nullptr) return;
    renderedFrame->Shutdown();
}

void DistortionSceneFrame::HandleEvent(GlEngine::Events::Event &evt)
{
    auto *kbdEvt = dynamic_cast<GlEngine::Events::KeyboardEvent*>(&evt);
    if (kbdEvt != nullptr && kbdEvt->GetEventType() == GlEngine::Events::KeyboardEventType::KeyTyped)
    {
        bool sr_changed = false;
        if (kbdEvt->GetVirtualKeyCode() == VK_SQUARE_BRACKET_RIGHT)
        {
            this->pair_index += 2;
            sr_changed = true;
        }
        else if (kbdEvt->GetVirtualKeyCode() == VK_SQUARE_BRACKET_LEFT)
        {
            this->pair_index -= 2;
            sr_changed = true;
        }
        if (sr_changed)
        {
            RefreshPair();
            kbdEvt->Handle();
        }
    }

    if (!evt.IsHandled() && renderedFrame != nullptr)
    {
        renderedFrame->HandleEvent(evt);
    }
}

void DistortionSceneFrame::RefreshPair()
{
    if (this->pair_index < 0)
        this->pair_index += PAIR_COUNT * 2;
    this->pair_index %= PAIR_COUNT * 2;

    this->currentSubroutine = subroutines[subroutineKernelPairs[pair_index]];
    this->currentConvolutionKernel = convolutionKernels[subroutineKernelPairs[pair_index + 1]];
    GlEngine::Util::Log(GlEngine::LogType::InfoC, "Setting pair to index %d", pair_index);
    GlEngine::Util::Log(GlEngine::LogType::InfoC, "Setting subroutine to %s", this->currentSubroutine->name().c_str());
    GlEngine::Util::Log(GlEngine::LogType::InfoC, "Setting kernel to index %d", subroutineKernelPairs[pair_index + 1]);
}
