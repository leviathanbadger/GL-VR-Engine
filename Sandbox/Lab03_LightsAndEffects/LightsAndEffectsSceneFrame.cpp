#include "stdafx.h"
#include "LightsAndEffectsSceneFrame.h"
#include "Tree.h"

#include "CameraGameObject.h"
#include "Lab3Controls.h"
#include "../CameraTargetObject.h"

#include "RandomUtils.h"
#include "../LightSourceObject.h"
#include "../TemplateTorus.h"
#include "PointLightSource.h"
#include "AmbientLightSource.h"
#include "FogSource.h"

#include "Property.h"
#include "Attribute.h"
#include "ShaderFactory.h"
#include "SpotlightSource.h"

#include "LogUtils.h"

typedef GlEngine::ShaderFactory::IPropertyProvider IPropertyProvider;
typedef GlEngine::PointLightSource PointLightSource;
typedef GlEngine::SpotlightSource SpotlightSource;

LightsAndEffectsSceneFrame::LightsAndEffectsSceneFrame()
{
}
LightsAndEffectsSceneFrame::~LightsAndEffectsSceneFrame()
{
}

bool LightsAndEffectsSceneFrame::Initialize()
{
    auto cameraTarget = this->CreateGameObject<CameraTargetObject>();

    auto cameraObject = this->CreateGameObject<GlEngine::CameraGameObject>();
    cameraObject->SetTargetObject(cameraTarget);
    cameraObject->SetLock(GlEngine::CameraLock::RELATIVE_POSITION);
    cameraObject->SetPosition({ 0, -3.5, 7 });

    CreateGameObject<Lab3Controls>();

    auto ambientLightSource = new GlEngine::AmbientLightSource({ .1f, .1f, .1f });
    auto lightGobj = CreateGameObject<LightSourceObject<PointLightSource>>(new PointLightSource());
    auto fog = new GlEngine::FogSource(0, 15.f, { 0.f, 0.f, 0.f, 1.f });
    lightGobj->lightSource()->SetPosition({ 0, 6.f, -6.f });

    for (int q = -2; q <= 2; q++)
    {
        for (int w = -2; w <= 2; w++)
        {
            float rndX = GlEngine::Util::random(2.f) - 1,
                  rndZ = GlEngine::Util::random(2.f) - 1;
            CreateGameObject<Tree>(Vector<3> { (q * 5) + rndX, 0, (w * 5) + rndZ }, std::vector<IPropertyProvider*> { ambientLightSource, lightGobj->lightSource(), fog });
        }
    }

    struct {
        Vector<3> color = { 1.0f, 0.2f, 1.0f };
        Vector<3> reflectionCoef = { 0.9f, 0.9f, 0.9f };
        Vector<3> rotationAxis = { 1, 0, 0 };
        float totalDelta = 0.0;
        float rotationSpeed = 0.5;
        float distance = 3.0;
        LightSourceObject<PointLightSource>* lightSource = nullptr;
    } celData;
    celData.lightSource = CreateGameObject<LightSourceObject<PointLightSource>>(new PointLightSource());

    auto cel = CreateGameObject<TemplateTorus>(
        new TemplateMaterial(
            {
                &GlEngine::ShaderFactory::prop_RgbColor,
                &GlEngine::ShaderFactory::prop_ReflectionCoefficient,
                &GlEngine::ShaderFactory::prop_CelLevels
            },
            {
                &GlEngine::ShaderFactory::attr_GlPosition,
                &GlEngine::ShaderFactory::attr_DiffuseOnly,
                &GlEngine::ShaderFactory::attr_RgbBaseColor,
                &GlEngine::ShaderFactory::attr_CelShading
            },
            [celData](TemplateMaterial*, GlEngine::ShaderFactory::ShaderFactory& factory)
            {
                factory.ProvideProperty(GlEngine::ShaderFactory::prop_RgbColor, celData.color);
                factory.ProvideProperty(GlEngine::ShaderFactory::prop_ReflectionCoefficient, celData.reflectionCoef);
                factory.ProvideProperty(GlEngine::ShaderFactory::prop_CelLevels, (int)Lab3Controls::celShadingSteps);
            }
        ), 
        std::vector<IPropertyProvider*> {
            celData.lightSource->lightSource()
        },
        [celData](TemplateTorus* self, float delta) mutable {
            if (LabControls::isPaused) return;
            else if (!!celData.rotationAxis[0] && !LabControls::rotateZ) return;
            else if (!!celData.rotationAxis[1] && !LabControls::rotateY) return;
            else if (!!celData.rotationAxis[2] && !LabControls::rotateX) return;

            celData.totalDelta += delta;
            auto rotationAmount = celData.totalDelta * celData.rotationSpeed;
            auto transformMatrix = !!celData.rotationAxis[0] ? Matrix<4, 4>::TranslateMatrix({ 0, celData.distance, 0 }) * Matrix<4, 4>::RollMatrix(rotationAmount)  * Matrix<4, 4>::TranslateMatrix(self->position) :
                                   !!celData.rotationAxis[1] ? Matrix<4, 4>::TranslateMatrix({ 0, 0, celData.distance }) * Matrix<4, 4>::YawMatrix(rotationAmount)   * Matrix<4, 4>::TranslateMatrix(self->position) :
                                                               Matrix<4, 4>::TranslateMatrix({ celData.distance, 0, 0 }) * Matrix<4, 4>::PitchMatrix(rotationAmount) * Matrix<4, 4>::TranslateMatrix(self->position);
            auto transformedPosition = transformMatrix.Transpose() * Vector<4>{ 0, 0, 0, 1 };
            celData.lightSource->lightSource()->SetPosition({ transformedPosition[0], transformedPosition[1], transformedPosition[2] });
        }
    );
    cel->SetPosition({ 20, 5, 0 });

    struct {
        Vector<3> ambient = { 0.1f, 0.1f, 0.1f };
        Vector<3> color = { 1.0f, 1.0f, 0.2f };
        Vector<3> reflectionCoef = { 0.9f, 0.9f, 0.9f };
        Vector<3> rotationAxis = { 0, 1, 0 };
        Vector<3> lightPosition = { -25, 0, 0 };
        float totalDelta = 0.0;
        float rotationSpeed = 0.5;
        float distance = 2.0;
        float shininess = 5.0;
        LightSourceObject<SpotlightSource>* lightSource = nullptr;
    } spotData;
    spotData.lightSource = CreateGameObject<LightSourceObject<SpotlightSource>>(new SpotlightSource({ 0, 1, 0 }, { 1.0, 1.0, 1.0 }, 3.0f, { 1, 0, 0 }, 10deg));

    for (int i = 0; i < 100; i++)
    {
        auto spotTorus = CreateGameObject<TemplateTorus>(
            new TemplateMaterial(
                {
                    &GlEngine::ShaderFactory::prop_RgbColor,
                    &GlEngine::ShaderFactory::prop_ReflectionCoefficient,
                    &GlEngine::ShaderFactory::prop_Shininess,
                    &GlEngine::ShaderFactory::prop_AmbientLightColor
                },
                {
                    &GlEngine::ShaderFactory::attr_GlPosition,
                    &GlEngine::ShaderFactory::attr_Phong,
                    &GlEngine::ShaderFactory::attr_RgbBaseColor,
                    &GlEngine::ShaderFactory::attr_Spotlight
                },
                [spotData](TemplateMaterial*, GlEngine::ShaderFactory::ShaderFactory& factory)
                {
                    factory.ProvideProperty(GlEngine::ShaderFactory::prop_RgbColor, spotData.color);
                    factory.ProvideProperty(GlEngine::ShaderFactory::prop_ReflectionCoefficient, spotData.reflectionCoef);
                    factory.ProvideProperty(GlEngine::ShaderFactory::prop_Shininess, spotData.shininess);
                    factory.ProvideProperty(GlEngine::ShaderFactory::prop_AmbientLightColor, spotData.ambient);
                }
            ),
            std::vector<IPropertyProvider*> {
                spotData.lightSource->lightSource()
            },
            [spotData, i](TemplateTorus* self, float delta) mutable {
                self;
                // Hacky way to only have this happen once
                if (i == 0)
                {
                    spotData.lightSource->lightSource()->SetAngleAttenuation(Lab3Controls::spotlightAttenuation);
                    spotData.lightSource->lightSource()->SetCutoffTheta(Lab3Controls::spotlightCutoffAngle);

                    if (LabControls::isPaused) return;
                    else if (!!spotData.rotationAxis[0] && !LabControls::rotateZ) return;
                    else if (!!spotData.rotationAxis[1] && !LabControls::rotateY) return;
                    else if (!!spotData.rotationAxis[2] && !LabControls::rotateX) return;

                    spotData.totalDelta += delta;
                    auto rotationAmount = spotData.totalDelta * spotData.rotationSpeed;
                    auto transformMatrix = !!spotData.rotationAxis[0] ? Matrix<4, 4>::TranslateMatrix({ 0, spotData.distance, 0 }) * Matrix<4, 4>::RollMatrix(rotationAmount)  * Matrix<4, 4>::TranslateMatrix(spotData.lightPosition) :
                                           !!spotData.rotationAxis[1] ? Matrix<4, 4>::TranslateMatrix({ 0, 0, spotData.distance }) * Matrix<4, 4>::YawMatrix(rotationAmount)   * Matrix<4, 4>::TranslateMatrix(spotData.lightPosition) :
                                                                        Matrix<4, 4>::TranslateMatrix({ spotData.distance, 0, 0 }) * Matrix<4, 4>::PitchMatrix(rotationAmount) * Matrix<4, 4>::TranslateMatrix(spotData.lightPosition);
                    auto transformedPosition = transformMatrix.Transpose() * Vector<4>{ 0, 0, 0, 1 };
                    Vector<3> tPos = { transformedPosition[0], transformedPosition[1], transformedPosition[2] };
                    auto relPosition = spotData.lightPosition - tPos;
                    auto direction = Vector<3>{ relPosition[0], relPosition[1], -relPosition[2] }.Normalized();

                    spotData.lightSource->lightSource()->SetPosition({ transformedPosition[0], transformedPosition[1], transformedPosition[2] });
                    spotData.lightSource->lightSource()->SetDirection(direction);
                }
            }
        );
        float rndX = GlEngine::Util::random(2.f) - 1,
              rndY = GlEngine::Util::random(2.f) - 1,
              rndZ = GlEngine::Util::random(2.f) - 1;
        spotTorus->SetPosition({ -25 + rndX * 10, rndY * 5, rndZ * 10 });
    }

    return true;
}
