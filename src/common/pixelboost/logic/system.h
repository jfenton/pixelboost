#pragma once

#include "pixelboost/db/definitions.h"
#include "pixelboost/graphics/renderer/common/renderable.h"

namespace pb
{
    
class Message;
class Scene;
class Viewport;

class SceneSystem
{
public:
    SceneSystem();
    virtual ~SceneSystem();
    
    virtual pb::Uid GetType() = 0;
    
    virtual void Update(Scene* scene, float time);
    virtual void Render(Scene* scene, Viewport* viewport, RenderPass renderPass);
    
    virtual void HandleMessage(Scene* scene, const Message& message);
};

}
