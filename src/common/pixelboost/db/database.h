#pragma once

#include <map>
#include <vector>

#include "pixelboost/db/struct.h"
#include "pixelboost/file/fileSystem.h"

struct lua_State;

namespace pb
{
    
class DbRecord;

struct DbRecordDescription
{
    std::string Name;
    Uid Type;
    Uid Id;
};

class Database
{
public:
    Database();
    ~Database();
    
    static Database* Instance();
    
public:
    lua_State* GetLuaState();
    
public:
    typedef void*(*CreateStruct)();
    typedef void(*DeserialiseStruct)(void* data);
    
    void RegisterCreate(Uid type, CreateStruct createStruct);
    void RegisterDeserialise(Uid type, DeserialiseStruct deserialiseStruct);
    
    void SetLocation(const std::string& location);
    void OpenDatabase();
    
    DbRecord* OpenRecord(Uid recordId);
    
    void* Create(Uid type);
    void Deserialise(Uid type, void* data);
    
public:
    typedef std::vector<DbRecordDescription> RecordDescriptionList;
    typedef std::map<Uid, DbRecord*> RecordMap;
    
    const RecordDescriptionList& GetRecordDescriptions() const;
    const RecordMap& GetRecords() const;
    const DbRecord* GetRecord(Uid uid) const;
    
private:
    FileLocation GetLocation();
    
    typedef std::map<Uid, CreateStruct> StructCreateMap;
    typedef std::map<Uid, DeserialiseStruct> StructDeserialiseMap;
    
    std::string _DatabaseRoot;
    
    RecordDescriptionList _RecordDescriptions;
    RecordMap _Records;
    
    StructCreateMap _StructCreate;
    StructDeserialiseMap _StructDeserialise;
    
    static Database* _Instance;
    
    lua_State* _State;
};
    
}
