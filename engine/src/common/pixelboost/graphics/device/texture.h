#pragma once

#include <string>

#include "glm/glm.hpp"

#include "pixelboost/file/fileHelpers.h"
#include "pixelboost/graphics/device/device.h"

namespace pb
{
    
class Texture
{
protected:
    Texture();
    virtual ~Texture();
    
public:
    enum TextureFormat
    {
        kTextureFormatRGB,
        kTextureFormatRGBA,
        kTextureFormatBGRA,
    };
    
    virtual bool LoadFromBytes(const unsigned char* data, int width, int height, bool createMips, TextureFormat format);
    virtual bool LoadFromFile(const std::string& image, bool createMips);
    
    const glm::vec2& GetSize() const;

#ifdef PIXELBOOST_GRAPHICS_HANDLE_CONTEXT_LOST
    void OnContextLost();
#endif
    
protected:
    virtual void Bind(int textureUnit = 0) = 0;
    
protected:
#ifdef PIXELBOOST_GRAPHICS_HANDLE_CONTEXT_LOST
    TextureFormat _DataFormat;
    unsigned char* _Data;
    bool _DataCreateMips;
#endif
    
    glm::vec2 _Size;
    
    friend class GraphicsDevice;
};

}

#ifdef PIXELBOOST_GRAPHICS_OPENGLES1
#include "pixelboost/graphics/device/gles1/texture.h"
#endif

#ifdef PIXELBOOST_GRAPHICS_OPENGL
#include "pixelboost/graphics/device/gl/texture.h"
#endif
