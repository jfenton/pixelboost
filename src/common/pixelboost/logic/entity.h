#pragma once

#include <map>
#include <vector>

#include "sigslot/signal.h"

#include "pixelboost/db/definitions.h"

namespace pb
{
    
class Component;
class Message;

class Entity
{
public:
    Entity(Uid uid);
    virtual ~Entity();
    
    Uid GetUid();
    virtual Uid GetType();    

public:
    typedef std::vector<Component*> ComponentList;
    
public:
    void AddComponent(Component* component);
    void RemoveComponent(Component* component);
    ComponentList* GetComponentsByType(Uid componentType);
    
    void SendMessage(Message& message);
    
    void RegisterMessageHandler(Uid messageType, sigslot::Delegate2<Uid, Message&> handler);
    void UnregisterMessageHandler(Uid messageType, sigslot::Delegate2<Uid, Message&> handler);
    
private:
    typedef std::map<Uid, sigslot::Signal2<Uid, Message&> > MessageHandlers;
    typedef std::map<Uid, ComponentList> ComponentMap;
    
    ComponentMap _Components;
    MessageHandlers _MessageHandlers;
    
    Uid _Uid;
};
    
}