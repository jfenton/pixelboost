#pragma once

#include <map>
#include <memory>
#include <string>

#include "glm/glm.hpp"

#include "pixelboost/graphics/device/texture.h"

namespace pb
{
    
class Sprite;
    
class SpriteSheet
{
private:
    SpriteSheet();
    
public:
    static std::shared_ptr<SpriteSheet> Create();
    virtual ~SpriteSheet();
    
    bool LoadSingle(const std::string& fileName, float density, bool generateMips=false);
    bool LoadSheet(const std::string& name, const std::string& extension, bool generateMips=false);
    Texture* LoadTexture(const std::string& fileName, bool generateMips);
    
    Sprite* GetSprite(const std::string& name);
    
public:
    Texture* _Texture;
    
private:
    typedef std::map<std::string, Sprite*> SpriteMap;
    SpriteMap _Sprites;
    
    friend class SpriteRenderer;
};

class Sprite
{
public:
    SpriteSheet* _Sheet;

    glm::vec2 _Size;
    glm::vec2 _Offset;
    
    glm::vec2 _UvSize;
    glm::vec2 _UvPosition;
    bool _Rotated;
};
    
}
