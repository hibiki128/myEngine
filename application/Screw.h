#pragma once

#include "ViewProjection.h"
#include "Liner.h"

#include <list>

class Screw
{
public:
    ~Screw();

    void Initialize();
    void Update();
    void Draw(const ViewProjection& _vp);

    Vector3 GetPositionOnLine(float _y)const;
    float GetHeight()const { return height_; }
    Vector3 GetCenter()const { return center_; }
    float GetHeightPerRot()const { return heightPerRot_; }
    std::vector<Vector3> GetStartPoint() { return startP; }
    std::vector<Vector3> GetEndPoint() { return endP; }
private:

    void Calculate();
    Vector3 CalculatePosition(float _t)const;

    std::list<Vector3> diviaionPoint_     = {};  // 描画できるような点
    std::vector<Liner*> lines_;

    std::vector<Vector3> startP;
    std::vector<Vector3> endP;
    Vector3     center_                     = {};   // 中点の座標
    Vector3     startPoint_                 = {};   // 始端の座標
    Vector3     endPoint_                   = {};   // 終端の座標

    float       heightPerRot_               = 1.0f; // 一回転当たりのｙの移動量
    float       minRadius_                  = 1.0f; // 最小半径
    float       maxRadius_                  = 3.0f; // 最大半径
    float       rotateDirection_            = 1.0f; // 回転の向き
    float       height_                     = 5.0f; // 中点から端までの距離
    float       divisionPerRot_             = 32.0f;// 一回転あたりの分割数
    float       theta_                       = 0.0f; 

    uint32_t    rotateNum_                  = 10;   // 回転数（始端から終端までの）
};