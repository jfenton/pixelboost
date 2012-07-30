#pragma once

#include <string>

#include "sigslot/signal.h"

#include "pixelboost/db/definitions.h"

namespace pb
{
    
class DbStruct
{
public:
    DbStruct(Uid uid, Uid type, void* _Data);
    virtual ~DbStruct();
    
    Uid GetType() const;
    Uid GetUid() const;
    
    void* GetData() const;
    template<class T> const T* GetData() const;
    
public:
    sigslot::Signal0<> structDestroyed;
    
public:
    Uid _Uid;
    Uid _Type;
    
    void* _Data;
};
    
}

#include "pixelboost/db/struct.inl"
