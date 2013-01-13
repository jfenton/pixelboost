#include "pixelboost/graphics/device/device.h"

#ifdef PIXELBOOST_GRAPHICS_OPENGL

#include "pixelboost/debug/log.h"
#include "pixelboost/graphics/device/gl/texture.h"

using namespace pb;

TextureGL::TextureGL(GraphicsDeviceGL* device)
    : _Device(device)
{
    
}

TextureGL::~TextureGL()
{
    glDeleteTextures(1, &_Texture);
}

bool TextureGL::LoadFromBytes(const unsigned char* data, int width, int height, bool createMips, TextureFormat format, bool hasPremultipliedAlpha)
{
    if (!Texture::LoadFromBytes(data, width, height, createMips, format, hasPremultipliedAlpha))
        return false;

    if (format != kTextureFormatRGBA && format != kTextureFormatRGB)
    {
        PbLogWarn("graphics.texture", "Only RGB and RGBA texture formats are currently supported!");
        return false;
    }
    
    glGenTextures(1, &_Texture);
    
    Texture* previousTexture = _Device->BindTexture(this, true);
    
    if (createMips)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    switch (format)
    {
        case Texture::kTextureFormatRGB:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);
            break;
        case Texture::kTextureFormatRGBA:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
            break;
    }

    if (createMips)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
	
    _Device->BindTexture(previousTexture);

    _Size = glm::vec2(width, height);
        
    return true;
}

void TextureGL::Bind(int unit)
{
    _Device->BindTexture(this);
}

#endif
