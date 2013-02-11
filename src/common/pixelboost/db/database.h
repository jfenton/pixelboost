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
    typedef void(*DestroyStruct)(void* structure);
    typedef void(*DeserialiseStruct)(Database* database, DbRecord* record, void* data);
    
    void RegisterCreate(Uid type, CreateStruct createStruct);
    void RegisterDestroy(Uid type, DestroyStruct destroyStruct);
    void RegisterDeserialise(Uid type, DeserialiseStruct deserialiseStruct);
    
    void SetLocation(const std::string& location);
    bool OpenDatabase();
    
    DbRecord* OpenRecord(Uid recordId);
    bool CloseRecord(Uid recordId);
    
    void* Create(Uid type);
    void Destroy(Uid type, void* structure);
    void Deserialise(Uid type, DbRecord* record, void* data);
	
	void AddReference(Uid uid, void** reference);
	void ResolveReferences();
    
public:
    typedef std::vector<DbRecordDescription> RecordDescriptionList;
    typedef std::map<Uid, DbRecord*> RecordMap;
    
    const RecordDescriptionList& GetRecordDescriptions() const;
    const RecordMap& GetRecords() const;
    const DbRecord* GetRecord(Uid uid) const;
    
private:
	struct RecordReference
    {
        Uid uid;
        void** reference;
    };
	
	typedef std::vector<RecordReference> ReferenceList;
    
    ReferenceList _References;
    bool _ResolvingReferences;
	
    typedef std::map<Uid, CreateStruct> StructCreateMap;
    typedef std::map<Uid, DestroyStruct> StructDestroyMap;
    typedef std::map<Uid, DeserialiseStruct> StructDeserialiseMap;
    
    std::string _DatabaseRoot;
    
    RecordDescriptionList _RecordDescriptions;
    RecordMap _Records;
    
    StructCreateMap _StructCreate;
    StructDestroyMap _StructDestroy;
    StructDeserialiseMap _StructDeserialise;
    
    static Database* _Instance;
    
    lua_State* _State;
};
    
}
