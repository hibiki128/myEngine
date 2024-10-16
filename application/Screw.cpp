#include "Screw.h"
#include "myMath.h"
#include <cmath>
#include "numbers"
#include "ImGuiManager.h"

Screw::~Screw()
{
    for (Liner* line : lines_)
    {
        delete line;
        line = nullptr;
    }
    lines_.clear();
}

void Screw::Initialize()
{
    center_ = { 0.0f,0.0f ,0.0f };

    heightPerRot_ = 1.0f;
    minRadius_ = 1.0f;
    maxRadius_ = 3.0f;
    rotateDirection_ = 1.0f;
    height_ = 5.0f;

    divisionPerRot_ = 32;
    rotateNum_ = 10;

    startPoint_ = { 0.0f,height_,minRadius_ };
    endPoint_ = { 0.0f,-height_,maxRadius_ };

    Calculate();
}

void Screw::Update() {
    startP.clear();
    endP.clear();
    ImGui::Begin("screw");
    ImGui::DragFloat3("center", &center_.x, 0.01f);
    ImGui::DragFloat("halfLength", &height_, 0.01f);
    ImGui::DragFloat("mimRadius", &minRadius_, 0.01f);
    ImGui::DragFloat("maxRadius", &maxRadius_, 0.01f);
	ImGui::DragFloat("heightInRotate", &heightPerRot_, 0.01f);
    ImGui::DragFloat("divisionPerRot", &divisionPerRot_, 1);
    ImGui::End();
    Calculate();
    for (auto it = diviaionPoint_.begin(); it != diviaionPoint_.end(); it++) {
        auto nextIt = std::next(it);
        if (nextIt == diviaionPoint_.end())
            break;
        startP.push_back(*it);
        endP.push_back(*nextIt);
    }
}

void Screw::Draw(const ViewProjection& _vp)
{
    uint32_t index = 0;
    for (auto it = diviaionPoint_.begin(); it != diviaionPoint_.end();it++) {
        auto nextIt = std::next(it);
        if (nextIt == diviaionPoint_.end())
            break;
        lines_[index++]->Draw(*it+center_, *nextIt + center_, _vp);
    }
}

Vector3 Screw::GetPositionOnLine(float _y) const
{
    float t = _y / height_;
    return CalculatePosition(t) + center_;
}


void Screw::Calculate()
{
    // 始端から終端へのベクトルを分割
    // 点を取りradを変えて線形補間

    diviaionPoint_.clear();

    startPoint_ = center_;
    endPoint_ = center_;
    startPoint_ += {0.0f, 0.0f, maxRadius_};
    endPoint_ += {0.0f, height_, minRadius_};

    Vector3 sub = endPoint_ - startPoint_;
    rotateNum_ = static_cast<uint32_t>(std::abs(std::floorf(sub.y / heightPerRot_)));
    // 中間の始点

    // 正面スタート
    endPoint_.z -= maxRadius_;



    // thetaで円周求める
    // 分割数でtを計算
    // 使ってyと半径を求める

    theta_ = std::numbers::pi_v<float> *2.0f / static_cast<float>(divisionPerRot_);
    for (float yDivi = 0; yDivi < rotateNum_; ++yDivi)
    {
        for (float i = 0; i < divisionPerRot_; ++i)
        {
            float t = (i + divisionPerRot_ * yDivi) / divisionPerRot_ / static_cast<float>(rotateNum_);
           /* float radius = Lerp(maxRadius_, minRadius_, t);
            float y = Lerp(endPoint_.y, startPoint_.y, t);

            float angle = std::numbers::pi_v<float> *2.0f * rotateNum_ * t;

            float x = std::cosf(angle) * radius;
            float z = std::sinf(angle) * radius;*/

            Vector3 offset = CalculatePosition(t);

            diviaionPoint_.push_back(offset);
        }
    }
    diviaionPoint_.reverse();
    /*for (float yDivi = 0; yDivi < rotateNum_; ++yDivi)
    {
        for (float i = 0; i < divisionPerRot_; ++i)
        {
            float t = (i + divisionPerRot_ * yDivi) / divisionPerRot_ / static_cast<float>(rotateNum_);
            float radius = Lerp(maxRadius_, minRadius_, t);
            float y = Lerp(midPoint_.y, endPoint_.y, t);

            float angle = kTheta * i * -1.0f;

            float x = std::cosf(angle) * radius;
            float z = std::sinf(angle) * radius;

            Vector3 offset = {x, y, z};

            diviaionPoint_.push_back(center_ + offset);
        }
    }*/


}

Vector3 Screw::CalculatePosition(float _t) const
{

    float radius = Lerp(maxRadius_, minRadius_, _t);
    float y = Lerp(startPoint_.y, endPoint_.y, _t);

    float angle = std::numbers::pi_v<float> *2.0f * rotateNum_ * _t;


    float x = std::cosf(angle) * radius;
    float z = std::sinf(angle) * radius;

    return Vector3(x, y, z);
}
