#pragma once

#include "libpixel/graphics/device/device.h"
#include "libpixel/graphics/device/texture.h"

#ifdef LIBPIXEL_GRAPHICS_OPENGLES1

#include "libpixel/graphics/device/gles1/device.h"

namespace libpixel
{

class TextureGLES1 : Texture
{
protected:
    TextureGLES1(GraphicsDeviceGLES1* _Device);
    virtual ~TextureGLES1();
    
public:
    void Load(const std::string& image, bool createMips);
    void Bind(int unit);
    
private:
    GraphicsDeviceGLES1* _Device;
    GLuint _Texture;
    
    friend class GraphicsDeviceGLES1;
};

}

#endif
