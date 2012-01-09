#include "pixelboost/data/json/reader.h"
#include "pixelboost/file/fileHelpers.h"
#include "pixelboost/graphics/device/texture.h"
#include "pixelboost/graphics/helper/screenHelpers.h"
#include "pixelboost/graphics/render/sprite/sprite.h"
#include "pixelboost/logic/game.h"

using namespace pixelboost;
    
std::shared_ptr<SpriteSheet> SpriteSheet::Create()
{
    return std::shared_ptr<SpriteSheet>(new SpriteSheet());
}

SpriteSheet::SpriteSheet()
{
}

SpriteSheet::~SpriteSheet()
{
    GraphicsDevice::Instance()->DestroyTexture(_Texture);
}

bool SpriteSheet::Load(const std::string& name, bool generateMips)
{
    std::string fileRoot = FileHelpers::GetRootPath();
    
    std::string jsonFilename = fileRoot + "/data/spritesheets/" + name + (ScreenHelpers::IsHighResolution() ? "-hd" : "") + ".json";
    
    std::string rootData = FileHelpers::FileToString(jsonFilename);
    
    json::Object root;
    
    json::Reader::Read(root, rootData);
    
    json::Object& meta = root["meta"];
    
    json::Object& size = meta["size"];
    json::Number& width = size["w"];
    json::Number& height = size["h"];
    
    json::Object& frames = root["frames"];
    
    for (json::Object::iterator it = frames.Begin(); it != frames.End(); ++it)
    {
        std::string name = it->name;
        json::Object data = it->element;
        
        json::Object frame = data["frame"];
        
        json::Number posX = frame["x"];
        json::Number posY = frame["y"];
        json::Number sizeX = frame["w"];
        json::Number sizeY = frame["h"];
        
        json::Boolean rotated = data["rotated"];
        
        Sprite* sprite = new Sprite();
        
        sprite->_Sheet = this;
        
        float ppu = 16.f;
        if (ScreenHelpers::IsHighResolution())
            ppu = 32.f;
        
        sprite->_Dimension = Vec2(sizeX.Value(), sizeY.Value()) / ppu;
        
        sprite->_Position = Vec2(posX.Value()/width.Value(), posY.Value()/height.Value());
        sprite->_Size = Vec2(sizeX.Value()/width.Value(), sizeY.Value()/height.Value());
        
        sprite->_Rotated = rotated.Value();
        
        _Sprites[name.substr(0, name.length()-4)] = sprite;
    }
    
    _Texture = GraphicsDevice::Instance()->CreateTexture();
    _Texture->Load(fileRoot + "/data/spritesheets/images/" + name + (ScreenHelpers::IsHighResolution() ? "-hd" : "") + ".png", generateMips);
    
    return (_Texture != 0);
}
    
Sprite* SpriteSheet::GetSprite(const std::string& name)
{
    SpriteMap::iterator it = _Sprites.find(name);
    
    if (it == _Sprites.end())
        return 0;
    
    return it->second;
}

Vec4 Sprite::GetUV()
{
    return Vec4(0, 0, 1, 1);
}