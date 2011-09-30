#include "libpixel/render/camera.h"
#include "libpixel/render/screenHelpers.h"
#include "libpixel/input/touchManager.h"

namespace libpixel
{

Vec2 Touch::GetScreenPosition()
{
    Vec2 position;
    
    if (ScreenHelpers::IsLandscape())
    {
        position[0] = 240 - _Position[1];
        position[1] = -(_Position[0] - 160);
    }
    else
    {
        position[0] = _Position[0] - 160;
        position[1] = 240 - _Position[1];
    }
    
	position[0] /= 15.f;
	position[1] /= 15.f;
	
	return position;
}

Vec2 Touch::GetWorldPosition(Camera* camera)
{
	Vec2 position;
    
    if (ScreenHelpers::IsLandscape())
    {
        position[0] = 240 - _Position[1];
        position[1] = -(_Position[0] - 160);
    }
    else
    {
        position[0] = _Position[0] - 160;
        position[1] = 240 - _Position[1];
    }
	
	position[0] /= 15.f;
	position[1] /= 15.f;
    
    position -= camera->Position;
	
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
{
    
}

void TouchManager::AddTouchHandler(TouchHandler* handler)
{
    _Handlers.insert(handler);
}
    
void TouchManager::RemoveTouchHandler(TouchHandler* handler)
{
    _Handlers.erase(handler);
}

void TouchManager::AddTouch(void* uiTouch, Vec2 position)
{
	Touch* touch = new Touch();
	touch->_Position = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) ? Vec2(position[0]/2.4f, position[1]/2.133f) : position;
	_Touches[uiTouch] = touch;
	OnTouchBegin(touch);
}

void TouchManager::RemoveTouch(void* touch)
{
	std::map<void*, Touch*>::iterator it = _Touches.find(touch);
	
	if (it != _Touches.end())
	{
		OnTouchEnd(it->second);
		delete it->second;
		_Touches.erase(it);
	}
}

void TouchManager::UpdateTouch(void* uiTouch, Vec2 position)
{
	Touch* touch = _Touches[uiTouch];
	touch->_Position = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) ? Vec2(position[0]/2.4f, position[1]/2.133f) : position;
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
        (*it)->OnTouchBegin(touch);
    }
}

void TouchManager::OnTouchUpdate(Touch* touch)
{
	for (TouchHandlerList::iterator it = _Handlers.begin(); it != _Handlers.end(); ++it)
    {
        (*it)->OnTouchUpdate(touch);
    }
}

void TouchManager::OnTouchEnd(Touch* touch)
{
	for (TouchHandlerList::iterator it = _Handlers.begin(); it != _Handlers.end(); ++it)
    {
        (*it)->OnTouchEnd(touch);
    }
}

}
