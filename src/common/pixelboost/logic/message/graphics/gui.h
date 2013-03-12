#pragma once

#include "pixelboost/logic/system/graphics/gui/gui.h"
#include "pixelboost/logic/message.h"

namespace pb
{
    
    class GuiComponent;
    class GuiRenderSystem;
    
    class GuiRenderMessage : public Message
    {
        PB_DECLARE_MESSAGE
        
    public:
        enum EventType
        {
            kEventTypeLayout,
            kEventTypeInput,
            kEventTypeRender,
        };
        
    public:
        GuiRenderMessage(GuiGlobalState& state, GuiRenderSystem* renderSystem, GuiComponent* guiComponent, EventType eventType, GuiInputEvent inputEvent = GuiInputEvent());
        virtual ~GuiRenderMessage();
        
        GuiGlobalState& GetState() const;
        
        GuiRenderSystem* GetGuiRenderSystem() const;
        GuiComponent* GetGuiComponent() const;
        
        EventType GetEventType() const;
        GuiInputEvent GetInputEvent() const;
        
    private:
        GuiGlobalState& _State;
        
        GuiRenderSystem* _GuiRenderSystem;
        GuiComponent* _GuiComponent;
        
        EventType _EventType;
        GuiInputEvent _InputEvent;
    };
    
}
