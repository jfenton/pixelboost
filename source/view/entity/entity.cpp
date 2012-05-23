#include "pixelboost/graphics/render/primitive/primitiveRenderer.h"

#include "core/selection.h"
#include "core/uidHelpers.h"
#include "project/entity.h"
#include "project/schema.h"
#include "view/entity/property/property.h"
#include "view/entity/property/sprite.h"
#include "view/entity/entity.h"
#include "core.h"
#include "view.h"

using namespace pixeleditor;

ViewEntity::ViewEntity(Uid uid, Entity* entity)
    : _Uid(uid)
    , _Entity(entity)
{
    ParseProperties();
    
    _BoundsDirty = true;
}

ViewEntity::~ViewEntity()
{
    
}

void ViewEntity::Update(float time)
{
    for (PropertyMap::iterator it = _Properties.begin(); it != _Properties.end(); ++it)
    {
        it->second->Update(time);
    }
}

void ViewEntity::Render(pb::RenderLayer* layer)
{
    UpdateBounds();
    
    Vec3 position = _Entity->GetPosition();
    
    for (PropertyMap::iterator it = _Properties.begin(); it != _Properties.end(); ++it)
    {
        it->second->Render(layer);
    }
    
    if (Core::Instance()->GetSelection().IsSelected(GenerateSelectionUid(GetUid())))
    {
        glm::vec3 boxCenter = _BoundingBox.GetCenter();
        glm::vec3 boxSize = _BoundingBox.GetSize();
        View::Instance()->GetPrimitiveRenderer()->AttachBox(layer, Vec2(boxCenter.x, boxCenter.y), Vec2(boxSize.x, boxSize.y), Vec3(0,0,0), Vec4(0.2,0.2,0.4,0.3));
           View::Instance()->GetPrimitiveRenderer()->AttachBox(layer, Vec2(boxCenter.x, boxCenter.y), Vec2(boxSize.x, boxSize.y), Vec3(0,0,0), Vec4(0.4,0.2,0.2,0.5), false);
    }
}

Uid ViewEntity::GetUid()
{
    return _Uid;
}

Entity* ViewEntity::GetEntity()
{
    return _Entity;
}

Vec3 ViewEntity::GetPosition()
{
    return _Entity->GetPosition();
}

pb::BoundingBox ViewEntity::GetBoundingBox()
{
    if (_BoundsDirty)
        UpdateBounds();
    
    return _BoundingBox;
}

Uid ViewEntity::GetPropertyIdByPath(const std::string& path)
{
    PropertyIdMap::iterator it = _PropertyIdMap.find(path);
    
    if (it != _PropertyIdMap.end())
    {
        return it->second;
    }
    
    return 0;
}

ViewProperty* ViewEntity::GetPropertyByPath(const std::string& path)
{
    PropertyIdMap::iterator it = _PropertyIdMap.find(path);
    
    if (it != _PropertyIdMap.end())
    {
        return GetPropertyById(it->second);
    }
    
    return 0;
}

ViewProperty* ViewEntity::GetPropertyById(Uid uid)
{
    PropertyMap::iterator it = _Properties.find(uid);
    if (it != _Properties.end())
    {
        return it->second;
    }
    
    return 0;
}

void ViewEntity::RemoveProperty(ViewProperty* property)
{
    RemovePropertyById(property->GetPropertyId());
}

void ViewEntity::RemovePropertyById(Uid uid)
{
    PropertyMap::iterator it = _Properties.find(uid);
    if (it != _Properties.end())
    {
        delete it->second;
        _Properties.erase(it);
    }
}

Uid ViewEntity::GeneratePropertyId(const std::string& path)
{
    Uid uid;
    
    do {
        uid = (rand()%((1<<12)-2))+1;
    } while (_Properties.find(uid) != _Properties.end());
    
    _PropertyIdMap[path] = uid;
    
    return uid;
}

Uid ViewEntity::AddProperty(ViewProperty* property)
{
    ViewProperty* other = GetPropertyByPath(property->GetPath());
    
    if (other)
    {
        RemoveProperty(other);
    }
    
    Uid uid = GeneratePropertyId(property->GetPath());
    
    _Properties[uid] = property;
    
    return uid;
}

void ViewEntity::ParseProperties()
{
    const SchemaStruct* schemaStruct = _Entity->GetType();
    
    ParseItem("/", schemaStruct);
    ParseStruct("/", schemaStruct);
}

void ViewEntity::ParseStruct(const std::string& path, const SchemaStruct* schemaStruct)
{
    for (SchemaStruct::PropertyMap::const_iterator it = schemaStruct->GetProperties().begin(); it != schemaStruct->GetProperties().end(); ++it)
    {
        std::string propertyPath = path + it->second->GetName() + "/";
        
        ParseItem(propertyPath, it->second);
        
        switch (it->second->GetPropertyType())
        {
            case SchemaProperty::kSchemaPropertyAtom:
            {
                break;
            }
            case SchemaProperty::kSchemaPropertyArray:
            {
                break;
            }
            case SchemaProperty::kSchemaPropertyStruct:
            {
                SchemaPropertyStruct* propertyStruct = static_cast<SchemaPropertyStruct*>(it->second);
                ParseStruct(propertyPath, propertyStruct->GetSchemaStruct());
                break;
            }
            case SchemaProperty::kSchemaPropertyPointer:
            {
                break;
            }
        }
    }
}

void ViewEntity::ParseItem(const std::string& path, const SchemaItem* item)
{
    if (const SchemaAttribute* visualisation = item->GetAttribute("Visualisation"))
    {
        std::string type = visualisation->GetParamValue("type");
        
        if (type == "sprite")
        {
            new SpriteViewProperty(this, path, item);
        }
    }
}

void ViewEntity::DirtyBounds()
{
    _BoundsDirty = true;
}

void ViewEntity::UpdateBounds()
{
    if (_BoundsDirty)
    {
        _BoundingBox.Invalidate();
        
        for (PropertyMap::iterator it = _Properties.begin(); it != _Properties.end(); ++it)
        {
            _BoundingBox.Expand(it->second->GetBoundingBox());
        }
        
        _BoundsDirty = false;
    }
}
