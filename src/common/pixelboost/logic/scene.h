#pragma once

#include <map>
#include <vector>

#include "pixelboost/db/definitions.h"
#include "pixelboost/graphics/renderer/common/renderable.h"

namespace pb
{
    
class DelayedMessage;
class Entity;
class Message;
class SceneSystem;
class Viewport;
    
class Scene
{
public:
    Scene();
    ~Scene();
    
    void Update(float time);
    void Render(Viewport* viewport, RenderPass renderPass);
    
    template <class T>T* GetSystemByType();
    bool AddSystem(SceneSystem* system);
    void RemoveSystem(SceneSystem* system);
    
    pb::Uid GenerateEntityId();
    
    void DestroyEntity(Entity* entity);
    void DestroyAllEntities();
    
    Entity* GetEntityById(Uid uid);
    
    void BroadcastMessage(const Message& message);
    void SendMessage(Uid uid, const Message& message);
    void BroadcastDelayedMessage(float delay, const Message* message);
    void SendDelayedMessage(Uid uid, float delay, const Message* message);
    
private:
    void AddEntity(Entity* entity);
    
    typedef std::pair<Uid, const Message*> DelayedMessage;
    typedef std::vector<std::pair<float, DelayedMessage> > DelayedMessageList;
    typedef std::map<Uid, Entity*> EntityMap;
    typedef std::map<Uid, SceneSystem*> SystemMap;
    
    DelayedMessageList _DelayedMessages;
    EntityMap _NewEntities;
    EntityMap _Entities;
    SystemMap _Systems;
    
    pb::Uid _NextFreeUid;
    
    friend class Entity;
};
    
}

#include "pixelboost/logic/scene.inl"
