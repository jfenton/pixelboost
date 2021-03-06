#include "pixelboost/file/fileSystem.h"
#include "pixelboost/network/networkServer.h"

#include "core.h"
#include "command/manager.h"
#include "project/project.h"

Core* Core::_Instance = 0;

Core::Core()
    : _Clipboard(0)
{
    _Instance = this;
    
    _CommandManager = new CommandManager();
    _Project = new Project();
    
    RegisterCoreCommands();
}

Core::~Core()
{
    _Instance = 0;
    
    delete _CommandManager;
    delete _Project;
}

Core* Core::Instance()
{
    return _Instance;
}

void Core::Update()
{
    
}

CommandManager* Core::GetCommandManager()
{
    return _CommandManager;
}

Project* Core::GetProject()
{
    return _Project;
}
    
Selection& Core::GetSelection()
{
    return _Selection;
}

void Core::SetSelection(const Selection& selection)
{
    _Selection = selection;
}

json::Object* Core::GetClipboard()
{
    return _Clipboard;
}

void Core::SetClipboard(const json::Object& clipboard)
{
    if (_Clipboard)
        delete _Clipboard;
    
    _Clipboard = new json::Object(clipboard);
}
