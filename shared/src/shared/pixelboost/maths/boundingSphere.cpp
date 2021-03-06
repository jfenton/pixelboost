#include "pixelboost/maths/boundingSphere.h"

using namespace pb;

BoundingSphere::BoundingSphere()
    : _Center(glm::vec3(0,0,0))
    , _Size(0)
    , _Valid(false)
{
}

BoundingSphere::BoundingSphere(glm::vec3 center, float size)
{
    Set(center, size);
}

bool BoundingSphere::IsValid() const
{
    return _Valid;
}

void BoundingSphere::Invalidate()
{
    _Valid = false;
}

glm::vec3 BoundingSphere::GetCenter() const
{
    return _Center;
}

float BoundingSphere::GetSize() const
{
    return _Size;
}

void BoundingSphere::Set(glm::vec3 center, float size)
{
    _Center = center;
    _Size = size;
    _Valid = true;
}

void BoundingSphere::Expand(glm::vec3 point)
{
    if (!_Valid)
    {
        _Center = point;
        _Size = 0.f;
        _Valid = true;
        return;
    }
    
    _Size = glm::max(_Size, glm::distance(_Center, point));
}

void BoundingSphere::Expand(const BoundingSphere& sphere)
{
    if (!_Valid)
    {
        _Center = sphere._Center;
        _Size = sphere._Size;
        _Valid = sphere._Valid;
        return;
    }
    
    if (sphere.IsValid())
    {
        glm::vec3 newCenter = (_Center + sphere._Center)/2.f;
        _Size = glm::max(glm::distance(newCenter, _Center)+_Size, glm::distance(newCenter, sphere._Center)+sphere._Size);
        _Center = newCenter;
    }
}

bool BoundingSphere::Contains(glm::vec3 point) const
{
    return glm::distance(point, _Center) < _Size;
}

bool BoundingSphere::Intersects(const BoundingSphere& box) const
{
    return glm::distance(_Center, box._Center) < _Size + box._Size;
}
