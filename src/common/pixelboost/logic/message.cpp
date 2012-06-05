#include "pixelboost/logic/message.h"

using namespace pb;

Message::Message(Entity* entity, Component* component)
    : _Entity(entity)
    , _Component(component)
{
    
}

Message::~Message()
{
    
}
