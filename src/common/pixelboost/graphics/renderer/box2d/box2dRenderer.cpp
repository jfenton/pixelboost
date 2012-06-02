#ifndef PIXELBOOST_DISABLE_BOX2D

#include "pixelboost/graphics/renderer/box2d/box2dRenderer.h"
#include "pixelboost/graphics/renderer/primitive/primitiveRenderer.h"
#include "pixelboost/logic/game.h"

namespace pb
{

Box2dRenderer::Box2dRenderer(RenderLayer* layer)
    : _Layer(layer)
{
    
}

Box2dRenderer::~Box2dRenderer()
{
    
}

void Box2dRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    for (int i=0; i<vertexCount-1; i++)
    {
        pb::Game::Instance()->GetPrimitiveRenderer()->AttachLine(_Layer, glm::vec2(vertices[i].x, vertices[i].y), glm::vec2(vertices[i+1].x, vertices[i+1].y), glm::vec4(color.r, color.g, color.b, 1.f));
    }
    
    if (vertexCount)
    {
        pb::Game::Instance()->GetPrimitiveRenderer()->AttachLine(_Layer, glm::vec2(vertices[vertexCount-1].x, vertices[vertexCount-1].y), glm::vec2(vertices[0].x, vertices[0].y), glm::vec4(color.r, color.g, color.b, 1.f));
    }
}

void Box2dRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    DrawPolygon(vertices, vertexCount, color);
}

void Box2dRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    pb::Game::Instance()->GetPrimitiveRenderer()->AttachEllipse(_Layer, glm::vec2(center.x, center.y), glm::vec2(radius, radius), glm::vec3(color.r, color.g, color.b));
}

void Box2dRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
    DrawCircle(center, radius, color);
}

void Box2dRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    pb::Game::Instance()->GetPrimitiveRenderer()->AttachLine(_Layer, glm::vec2(p1.x, p1.y), glm::vec2(p2.x, p2.y), glm::vec4(color.r, color.g, color.b, 1.f));
}

void Box2dRenderer::DrawTransform(const b2Transform& xf)
{
    
}

}

#endif