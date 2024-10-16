#pragma once

#include "Vector3.h"

#include <string>
#include <cstdint>
#include <initializer_list>

class Collider 
{
public:
    enum class BoundingBox : uint32_t
    {
        Sphere_3D = 0,
        Box_3D = 1
    };

    virtual Vector3 GetWorldPosition() = 0;
    virtual void OnCollision() = 0;

    float GetRadius()const { return radius_; }
    Vector3 GetSize()const { return size_; }

    void SetBoundingBox(BoundingBox _set) { boundingBox_ = _set; }
    BoundingBox GetBoundingBox() { return boundingBox_; }

    void SetAtrribute(const std::string& _atrribute);
    void SetMask(const std::string& _atrribute);
    void SetMask(std::initializer_list<std::string> _atrribute);
    uint32_t GetMask() { return mask_; }
    uint32_t GetAtrribute_() { return atrribute_; }


protected:
    // 衝突判定用変数
    Vector3 size_;  // AABB
    float radius_;  // 球

    BoundingBox boundingBox_ = BoundingBox::Sphere_3D;
    uint32_t atrribute_ = 0x0;
    uint32_t mask_ = 0x1;

};