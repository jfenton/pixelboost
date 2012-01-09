#include "pixelboost/graphics/device/device.h"
#include "pixelboost/graphics/render/custom/customRenderer.h"

namespace pixelboost
{

CustomRendererItem::~CustomRendererItem()
{

}
    
CustomRenderer::CustomRenderer(Renderer* renderer)
    : IRenderer(renderer)
{
    
}
    
CustomRenderer::~CustomRenderer()
{
    
}

void CustomRenderer::Update(float time)
{
    _Items.clear();
}

void CustomRenderer::Render(RenderLayer* layer)
{
    ItemList& list = _Items[layer];
    
    if (!list.size())
        return;
    
    for (ItemList::iterator it = list.begin(); it != list.end(); ++it)
    {
        (*it)->OnCustomRender();
    }
}

void CustomRenderer::AttachItem(RenderLayer* layer, CustomRendererItem* item)
{
    _Items[layer].push_back(item);
}
    
}
