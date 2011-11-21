#include "libpixel/graphics/device/device.h"
#include "libpixel/graphics/device/indexBuffer.h"
#include "libpixel/graphics/device/vertexBuffer.h"
#include "libpixel/graphics/render/sprite/spriteRenderer.h"
#include "libpixel/graphics/render/sprite/sprite.h"

namespace libpixel
{

SpriteInstance::SpriteInstance(Sprite* sprite) :
	  sprite(sprite)
{
}
    
SpriteInstance::~SpriteInstance()
{
}

SpriteRenderer::SpriteRenderer(int maxSpritesPerLayer, int numVertexBuffers)
    : _CurrentVertexBuffer(0)
    , _MaxSprites(maxSpritesPerLayer)
{
    _IndexBuffer = libpixel::GraphicsDevice::Instance()->CreateIndexBuffer(libpixel::kBufferFormatStatic, _MaxSprites*6);
    
    for (int i=0; i<numVertexBuffers; i++)
    {
        VertexBuffer* vertexBuffer = libpixel::GraphicsDevice::Instance()->CreateVertexBuffer(libpixel::kBufferFormatDynamic, libpixel::kVertexFormat_P_XYZ_UV, _MaxSprites*4);
        
        _VertexBuffers.push_back(vertexBuffer);
    }
    
    _IndexBuffer->Lock();
    unsigned short* indexBuffer = _IndexBuffer->GetData();
    for (int i=0; i<_MaxSprites; i++)
    {
        indexBuffer[0] = (i*4);
        indexBuffer[1] = (i*4) + 1;
        indexBuffer[2] = (i*4) + 2;
        indexBuffer[3] = (i*4) + 0;
        indexBuffer[4] = (i*4) + 2;
        indexBuffer[5] = (i*4) + 3;
        
        indexBuffer += 6;
    }
    _IndexBuffer->Unlock();
}

SpriteRenderer::~SpriteRenderer()
{
    for (VertexBufferList::iterator it = _VertexBuffers.begin(); it != _VertexBuffers.end(); ++it)
    {
        libpixel::GraphicsDevice::Instance()->DestroyVertexBuffer(*it);
    }
    libpixel::GraphicsDevice::Instance()->DestroyIndexBuffer(_IndexBuffer);
}
    
void SpriteRenderer::Update(float time)
{
    _Instances.clear();
}
    
void SpriteRenderer::Render(RenderLayer* layer)
{
    InstanceList& instanceList = _Instances[layer];
    if (!instanceList.size())
        return;
    
    // TODO: Remove this check and simply refill vertex buffer
    if (instanceList.size() > _MaxSprites)
        return;
    
    VertexBuffer* vertexBuffer = _VertexBuffers[_CurrentVertexBuffer];
    
    _CurrentVertexBuffer++;
    
    if (_CurrentVertexBuffer >= _VertexBuffers.size())
        _CurrentVertexBuffer = 0;
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    
    _IndexBuffer->Bind();
        
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    vertexBuffer->Lock();
    
    Vertex_PXYZ_UV* bufferData = static_cast<Vertex_PXYZ_UV*>(vertexBuffer->GetData());
    
    // TODO: Switch and draw elements when sheet changes
    instanceList[0].sprite->_Sheet->_Texture->Bind(0);
    switch (instanceList[0].blendMode)
    {
        case kBlendModeMultiply:
            glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case kBlendModeScreen:
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
            break;
        case kBlendModeAdd:
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        case kBlendModeOverlay:
            glBlendFunc(GL_DST_COLOR, GL_ONE);
            break;
        case kBlendModeNormal:
#ifdef LIBPIXEL_GRAPHICS_PREMULTIPLIED_ALPHA
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#else
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
            break;
    }
    
    for (InstanceList::iterator it = instanceList.begin(); it != instanceList.end(); ++it)
    {
        bufferData[0].position[0] = (cos(it->rotation[2]-M_PI_4)*it->scale[0])+it->position[0]; // Top Left
        bufferData[0].position[1] = (sin(it->rotation[2]-M_PI_4)*it->scale[1])+it->position[1];
        bufferData[0].position[2] = 0.f;
        bufferData[1].position[0] = (cos(it->rotation[2]+M_PI_2-M_PI_4)*it->scale[0])+it->position[0]; // Top Right
        bufferData[1].position[1] = (sin(it->rotation[2]+M_PI_2-M_PI_4)*it->scale[1])+it->position[1];
        bufferData[1].position[2] = 0.f;
        bufferData[2].position[0] = (cos(it->rotation[2]+M_PI-M_PI_4)*it->scale[0])+it->position[0]; // Bottom Right
        bufferData[2].position[1] = (sin(it->rotation[2]+M_PI-M_PI_4)*it->scale[1])+it->position[1];
        bufferData[2].position[2] = 0.f;
        bufferData[3].position[0] = (cos(it->rotation[2]-M_PI_2-M_PI_4)*it->scale[0])+it->position[0]; // Bottom Left
        bufferData[3].position[1] = (sin(it->rotation[2]-M_PI_2-M_PI_4)*it->scale[1])+it->position[1];
        bufferData[3].position[2] = 0.f;
                
        if (!it->sprite->_Rotated)
        {
            Vec2 min = it->sprite->_Position + Vec2(it->sprite->_Size[0] * it->crop[0], it->sprite->_Size[1] * it->crop[1]);
            Vec2 max = it->sprite->_Position + Vec2(it->sprite->_Size[0] * it->crop[2], it->sprite->_Size[1] * it->crop[3]);
            
            bufferData[0].uv[0] = max[0];
            bufferData[0].uv[1] = max[1],
            bufferData[1].uv[0] = max[0];
            bufferData[1].uv[1] = min[1];
            bufferData[2].uv[0] = min[0];
            bufferData[2].uv[1] = min[1];
            bufferData[3].uv[0] = min[0];
            bufferData[3].uv[1] = max[1];
        } else {
            Vec2 min = it->sprite->_Position + Vec2(it->sprite->_Size[1] * it->crop[3], it->sprite->_Size[0] * it->crop[2]);
            Vec2 max = it->sprite->_Position + Vec2(it->sprite->_Size[1] * it->crop[1], it->sprite->_Size[0] * it->crop[0]);
            
            bufferData[0].uv[0] = max[0];
            bufferData[0].uv[1] = min[1];
            bufferData[1].uv[0] = min[0];
            bufferData[1].uv[1] = min[1];
            bufferData[2].uv[0] = min[0];
            bufferData[2].uv[1] = max[1];
            bufferData[3].uv[0] = max[0];
            bufferData[3].uv[1] = max[1];
        }
        
        bufferData += 4;
    }
    
    vertexBuffer->Unlock(instanceList.size()*4);
    vertexBuffer->Bind();
    
    GraphicsDevice::Instance()->DrawElements(GraphicsDevice::kElementTriangles, instanceList.size()*6);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

bool SpriteRenderer::AddSpriteSheet(const std::string& name, libpixel::shared_ptr<SpriteSheet> spriteSheet)
{
    SheetMap::iterator it = _SpriteSheets.find(name);
    
    if (it != _SpriteSheets.end())
        return true;
    
    _SpriteSheets[name] = spriteSheet;

    return true;
}
 
bool SpriteRenderer::RemoveSpriteSheet(libpixel::shared_ptr<SpriteSheet> spriteSheet)
{
    for (SheetMap::iterator it = _SpriteSheets.begin(); it != _SpriteSheets.end(); ++it)
    {
        if (it->second == spriteSheet)
        {
            _SpriteSheets.erase(it);
            return true;
        }
    }
    
    return false;
}

bool SpriteRenderer::AttachToRenderer(RenderLayer* layer, const std::string& sheetName, const std::string& spriteName, Vec2 position, Vec3 rotation, Vec2 scale, BlendMode blendMode, Vec4 crop)
{
    Sprite* sprite = GetSprite(sheetName, spriteName);
    
    if (!sprite)
        return false;

    SpriteInstance instance(sprite);
    
    instance.crop = crop;
    instance.position = position - Vec2((0.5 - (crop[0]+crop[2])/2.f) * sprite->_Dimension[0], (0.5 - (crop[1]+crop[3])/2.f) * sprite->_Dimension[1]);
    instance.rotation = (rotation / 180.f) * M_PI;
    instance.scale = sprite->_Dimension * Vec2(crop[2]-crop[0], crop[3]-crop[1]) * scale * sqrt(2) * 0.5; // Scale now for later optimisation
    
    instance.blendMode = blendMode;
    
    _Instances[layer].push_back(instance);
    
    return true;
}
    
Sprite* SpriteRenderer::GetSprite(const std::string &sheetName, const std::string &spriteName) const
{
    libpixel::shared_ptr<SpriteSheet> sheet = GetSpriteSheet(sheetName);
    
    if (!sheet)
        return 0;
    
    return sheet->GetSprite(spriteName);
}

libpixel::shared_ptr<SpriteSheet> SpriteRenderer::GetSpriteSheet(const std::string& spriteSheet) const
{
    SheetMap::const_iterator it = _SpriteSheets.find(spriteSheet);
    
    if (it == _SpriteSheets.end())
        return libpixel::shared_ptr<SpriteSheet>();
    
    return it->second;
}

}
