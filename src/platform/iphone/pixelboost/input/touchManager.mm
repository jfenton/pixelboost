#ifndef PIXELBOOST_DISABLE_GRAPHICS

#ifdef PIXELBOOST_PLATFORM_IOS

#include "pixelboost/graphics/camera/camera.h"
#include "pixelboost/graphics/helper/screenHelpers.h"
#include "pixelboost/input/touchManager.h"

namespace pb
{
    
glm::vec2 Touch::GetDisplayPosition()
{
    return _Position;
}

glm::vec2 Touch::GetScreenPosition()
{
   	glm::vec2 position = _Position;
    
    // Specifically handle retina displays where the input co-ordinates aren't scaled
    float inputScale = ScreenHelpers::IsHighResolution() ? 2 : 1;
    
    position.x = position.x - ScreenHelpers::GetScreenResolution().x/(2*inputScale);
    position.y = ScreenHelpers::GetScreenResolution()[1]/(2*inputScale) - position[1];
    
	position.x /= (ScreenHelpers::GetDpu()/inputScale);
	position.y /= (ScreenHelpers::GetDpu()/inputScale);
	
	return position;
}

glm::vec2 Touch::GetWorldPosition(OrthographicCamera* camera)
{
	glm::vec2 position = _Position;
    
    // Specifically handle retina displays where the input co-ordinates aren't scaled
    float inputScale = ScreenHelpers::IsHighResolution() ? 2 : 1;
    
    position[0] = position[0] - ScreenHelpers::GetScreenResolution()[0]/(2*inputScale);
    position[1] = ScreenHelpers::GetScreenResolution()[1]/(2*inputScale) - position[1];

	position[0] /= (ScreenHelpers::GetDpu()/inputScale);
	position[1] /= (ScreenHelpers::GetDpu()/inputScale);
    
    position /= camera->Scale;
    position += glm::vec2(camera->Position.x, camera->Position.y);
	
	return position;
}

void TouchHandler::OnTouchBegin(Touch* touch)
{
}

void TouchHandler::OnTouchUpdate(Touch* touch)
{
}

void TouchHandler::OnTouchEnd(Touch* touch)
{
}
    
TouchManager::TouchManager()
    : _IsTouchActive(false)
{
    
}

void TouchManager::AddTouchHandler(TouchHandler* handler)
{
    TouchHandlerList::iterator it = _Handlers.find(handler);
    if (it != _Handlers.end())
    {
        it->second = true;
    } else {
        _HandlersToAdd[handler] = true;
    }
}
    
void TouchManager::RemoveTouchHandler(TouchHandler* handler)
{
    TouchHandlerList::iterator it = _Handlers.find(handler);
    if (it != _Handlers.end())
    {
        it->second = false;
    }
    
    it = _HandlersToAdd.find(handler);
    if (it != _HandlersToAdd.end())
    {
        it->second = false;
    }
}

void TouchManager::AddTouch(void* uiTouch, glm::vec2 position)
{
    UpdateTouchHandlers();
    
	Touch* touch = new Touch();
	touch->_Position = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) ? position/2.f : position;
	_Touches[uiTouch] = touch;
	OnTouchBegin(touch);
}

void TouchManager::RemoveTouch(void* touch)
{
    UpdateTouchHandlers();
    
	std::map<void*, Touch*>::iterator it = _Touches.find(touch);
	
	if (it != _Touches.end())
	{
		OnTouchEnd(it->second);
		delete it->second;
		_Touches.erase(it);
	}
}

void TouchManager::UpdateTouch(void* uiTouch, glm::vec2 position)
{
    UpdateTouchHandlers();
    
	Touch* touch = _Touches[uiTouch];
	touch->_Position = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) ? position/2.f : position;
	OnTouchUpdate(touch);
}

int TouchManager::GetNumTouches()
{
	return _Touches.size();
}

Touch* TouchManager::GetTouch(int index)
{
	std::map<void*, Touch*>::iterator it = _Touches.begin();
	
	while (index && it != _Touches.end())
	{
		it++;
		index--;
	}
	
	if (it == _Touches.end())
		return 0;
	
	return it->second;
}

void TouchManager::OnTouchBegin(Touch* touch)
{
    for (TouchHandlerList::iterator it = _Handlers.begin(); it != _Handlers.end(); ++it)
    {
        if (it->second)
            it->first->OnTouchBegin(touch);
    }
}

void TouchManager::OnTouchUpdate(Touch* touch)
{
	for (TouchHandlerList::iterator it = _Handlers.begin(); it != _Handlers.end(); ++it)
    {
        if (it->second)
            it->first->OnTouchUpdate(touch);
    }
}

void TouchManager::OnTouchEnd(Touch* touch)
{
	for (TouchHandlerList::iterator it = _Handlers.begin(); it != _Handlers.end(); ++it)
    {
        if (it->second)
            it->first->OnTouchEnd(touch);
    }
}
    
void TouchManager::UpdateTouchHandlers()
{
    for (TouchHandlerList::iterator it = _Handlers.begin(); it != _Handlers.end();)
    {
        if (it->second == false)
        {
            _Handlers.erase(it);
            it = _Handlers.begin();
        } else {
            ++it;
        }
    }
    
    for (TouchHandlerList::iterator it = _HandlersToAdd.begin(); it != _HandlersToAdd.end(); ++it)
    {
        if (it->second == true)
        {
            _Handlers[it->first] = it->second;
        }
    }
    _HandlersToAdd.clear();
}

}

#endif

#endif
