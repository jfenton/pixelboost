#pragma once

#include <vector>

#include "glm/glm.hpp"

namespace pb
{

class CustomRenderer;
class DebugDatabaseHandler;
class FontRenderer;
class GameCenter;
class GraphicsDevice;
class KeyboardManager;
class ModelRenderer;
class MouseManager;
class NetworkServer;
class ParticleRenderer;
class PrimitiveRenderer;
class Renderer;
class ResourceManager;
class Screen;
class SoundManager;
class SpriteRenderer;
class TouchManager;
    
class Game
{
public:
	Game(void* viewController);
	virtual ~Game();
	
	static Game* Instance();
	
	virtual void Initialise();

    CustomRenderer* GetCustomRenderer() const;
    FontRenderer* GetFontRenderer() const;
    GameCenter* GetGameCenter() const;
    ModelRenderer* GetModelRenderer() const;
	ResourceManager* GetResourceManager() const;
    ParticleRenderer* GetParticleRenderer() const;
    PrimitiveRenderer* GetPrimitiveRenderer() const;
    Renderer* GetRenderer() const;
	SpriteRenderer* GetSpriteRenderer() const;
    
    KeyboardManager* GetKeyboardManager() const;
    MouseManager* GetMouseManager() const;
	TouchManager* GetTouchManager() const;
    
    float GetGameTime();
    float GetTotalTime();
    
    virtual bool IsLandscape();
    virtual glm::vec2 GetScreenResolution();
    
    virtual void OnMemoryWarning();
	
	virtual void Update(float time);
	virtual void Render();
    
    void* GetViewController();
	
private:
	static Game* _Instance;

    CustomRenderer* _CustomRenderer;
    FontRenderer* _FontRenderer;
    GameCenter* _GameCenter;
    ModelRenderer* _ModelRenderer;
    ParticleRenderer* _ParticleRenderer;
    PrimitiveRenderer* _PrimitiveRenderer;
    Renderer* _Renderer;
	ResourceManager* _ResourceManager;
	SoundManager* _SoundManager;
	SpriteRenderer* _SpriteRenderer;
    
    KeyboardManager* _KeyboardManager;
    MouseManager* _MouseManager;
	TouchManager* _TouchManager;
    
    NetworkServer* _DebugNetwork;
    DebugDatabaseHandler* _DebugDatabaseHandler;
	
	float _GameTime;
    float _TotalTime;

    void* _ViewController;
};
    
}
