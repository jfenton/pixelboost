#include "pixelboost/logic/message/particle/finished.h"

using namespace pb;

PB_DEFINE_MESSAGE(pb::ParticleFinishedMessage)

ParticleFinishedMessage::ParticleFinishedMessage(Entity* entity, Component* component)
    : Message(entity, component)
{
    
}

ParticleFinishedMessage::~ParticleFinishedMessage()
{
    
}
