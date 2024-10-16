#pragma once

#include "WorldTransform.h"
#include "Object3d.h"
#include "Collider.h"

class Enemy :public Collider
{
public:
    void Initialize();
    void Update();
    void Draw(const ViewProjection& _vp);

    Vector3 GetWorldPosition();
    void OnCollision();

private:

    std::unique_ptr<Object3d>           obj_                = nullptr;

    WorldTransform                      worldTransform_     = {};

    uint32_t                            hp_                 = 0;
    uint32_t                            maxHp_              = 30;
    bool                                isAlive_            = false;

};