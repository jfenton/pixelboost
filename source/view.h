#pragma once

#include <string>
#include <vector>

#include "glm/glm.hpp"
#include "Gwen/Events.h"
#include "sigslot/signal.h"

#include "pixelboost/framework/engine.h"
#include "pixelboost/resource/resourceManager.h"

#include "core/selection.h"

namespace Gwen
{
    namespace Controls
    {
        class Base;
        class Canvas;
        class CollapsibleCategory;
        class CollapsibleList;
        class ListBox;
        class MenuItem;
        class MenuStrip;
    }
}

namespace pb
{
    class GwenInputHandler;
    class GwenRenderable;
    class GwenRenderer;
    class MaterialResource;
    class Model;
    class OrthographicCamera;
    class RenderLayer;
    class Scene;
    class Viewport;
}

class CommandManager;
class Core;
class HttpInterface;
class Level;
class ManipulatorManager;
class Project;
class ProjectRecord;
class ViewEntity;
class ViewKeyboardHandler;
class ViewMouseHandler;

class View : public pb::Engine, public Gwen::Event::Handler
{
public:
    View(void* platformContext, int argc, const char** argv);
    ~View();
    
public:
    static View* Instance();
    virtual void Initialise();
    virtual void Update(float timeDelta, float gameDelta);
    
    ManipulatorManager* GetManipulatorManager();
    
    void SetDirty();
    
    void Scroll(glm::vec2 offset);
    void Zoom(float delta);
    
public:
    void SetCanvasSize(glm::vec2 size);
    void SetRecord(ProjectRecord* record);
    ProjectRecord* GetRecord();
    
    pb::Scene* GetLevelScene();
    Level* GetLevel();
    pb::OrthographicCamera* GetLevelCamera();
    
public:
    sigslot::Signal0<> onRedraw;
    
private:
    void OnDisplayResolutionChanged(glm::vec2 resolution);
    void OnDisplayDensityChanged(float density);
    
private:
    ProjectRecord* _Record;
    
private:
    void OnProjectOpened(Project* project);
    void OnProjectClosed(Project* project);
    void OnProjectSaved(Project* project);
    void OnProjectExported(Project* project);
    
    void OnRecordAdded(Project* project, ProjectRecord* record);
    void OnRecordRemoved(Project* project, ProjectRecord* record);
    
    void OnRecordSelected(Gwen::Controls::Base* item);
    
    void OnEntityAdded(ViewEntity* entity);
    void OnEntityRemoved(ViewEntity* entity);
    
    void OnEntitySelected(Gwen::Controls::Base* item);
    void OnEntityCreate(Gwen::Controls::Base* item);
    
    void OnDeviceAddress(Gwen::Controls::Base* item);
    void OnCreateRecord(Gwen::Controls::Base* item);
    void OnMoveManipulator(Gwen::Controls::Base* item);
    void OnRotateManipulator(Gwen::Controls::Base* item);
    
    void OnSelectionChanged();
    
    void SetupCreateMenu(Project* project);
    
private:
    ManipulatorManager* _ManipulatorManager;
    
    Level* _Level;
    
    pb::OrthographicCamera* _LevelCamera;
    pb::Scene* _LevelScene;
    pb::Viewport* _LevelViewport;
    
    pb::OrthographicCamera* _UiCamera;
    pb::Scene* _UiScene;
    pb::Viewport* _UiViewport;
    
    pb::GwenInputHandler* _GwenInput;
    Gwen::Controls::Canvas* _GwenCanvas;
    pb::GwenRenderable* _GwenRenderable;
    pb::GwenRenderer* _GwenRenderer;
    std::shared_ptr<pb::MaterialResource> _GwenMaterial;

    Gwen::Controls::MenuStrip* _Menu;
    Gwen::Controls::MenuItem* _CreateMenu;
    Gwen::Controls::CollapsibleList* _FilePage;
    Gwen::Controls::CollapsibleCategory* _Records;    
    Gwen::Controls::CollapsibleList* _EntityPage;
    Gwen::Controls::CollapsibleCategory* _Entities;
    Gwen::Controls::ListBox* _Output;
    
    ViewKeyboardHandler* _KeyboardHandler;
    ViewMouseHandler* _MouseHandler;
    
    Core* _Core;
    int _LaunchArgc;
    const char** _LaunchArgv;
};
