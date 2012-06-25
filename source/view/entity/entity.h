#pragma once

#include <map>
#include <string>

#include "glm/glm.hpp"

#include "pixelboost/logic/entity.h"
#include "pixelboost/maths/boundingBox.h"

#include "project/definitions.h"

namespace pb
{
    class RectangleComponent;
    class RenderLayer;
}

namespace pixeleditor
{
    class Entity;
    class SchemaItem;
    class SchemaStruct;
    class Selection;
    class Struct;
    class ViewProperty;
    
    class ViewEntity : public pb::Entity
    {
    public:
        ViewEntity(pb::Scene* scene, Uid uid, pixeleditor::Entity* entity);
        ~ViewEntity();
        
        void Update(float time);
        void Render(int layer);
        
        pixeleditor::Entity* GetEntity();
        
    public:
        glm::vec3 GetPosition();
        glm::vec3 GetRotation();
        glm::vec3 GetScale();
        
        void SetPosition(glm::vec3 position);
        void SetRotation(glm::vec3 rotation);
        void SetScale(glm::vec3 scale);
        
        void Transform(glm::vec3 positionOffset, glm::vec3 rotationOffset = glm::vec3(0,0,0), glm::vec3 scaleOffset = glm::vec3(1,1,1));
        
        void CommitTransform();
        void ResetTransform();
        
    public:
        pb::BoundingBox GetBoundingBox();
        
        Uid GetPropertyIdByPath(const std::string& path);
        ViewProperty* GetPropertyByPath(const std::string& path);
        ViewProperty* GetPropertyById(Uid uid);

    private:
        void OnSelectionChanged(const Selection* selection);
        
        Uid AddProperty(ViewProperty* property);
        void RemoveProperty(ViewProperty* property);
        void RemovePropertyById(Uid uid);
        
        Uid GeneratePropertyId(const std::string& path);
        
        void ParseProperties();
        void ParseStruct(const std::string& path, const SchemaStruct* schemaStruct);
        void ParseItem(const std::string& path, const SchemaItem* item);
        
        void OnDestroyed(Struct* structure);
        void OnPropertyChanged(Struct* structure);
        
        void DirtyBounds();
        void UpdateBounds();
        
    private:
        typedef std::map<std::string, Uid> PropertyIdMap;
        typedef std::map<Uid, ViewProperty*> PropertyMap;
        
    private:
        pixeleditor::Entity* _Entity;
        PropertyIdMap _PropertyIdMap;
        PropertyMap _Properties;
        
        bool _BoundsDirty;
        pb::BoundingBox _BoundingBox;
        pb::RectangleComponent* _BoundsComponent;
        
        friend class ViewProperty;
    };
}
