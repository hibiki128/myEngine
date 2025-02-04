#pragma once
#include "Sprite.h"
#include "externals/nlohmann/json.hpp"
#include <memory>
class UI {
  public:
    void Init();
    void Update();
    void Draw();
    void Debug();
    void SetStart(bool flag) { isStart_ = flag; }
    bool IsFinish() { return isFinish_; }
    void SetTexture(std::string filePath) { game_->SetTexturePath(filePath); }
    void SetEneNum(int num) { enemyNum = num; }

  private:
    void TextMove();
    void SaveToJson();
    void LoadFromJson();
    void Number();

  private:
    using json = nlohmann::json;

    std::unique_ptr<Sprite> explanation_;
    std::unique_ptr<Sprite> explanation2_;
    std::unique_ptr<Sprite> singleDigit_;
    std::unique_ptr<Sprite> twoDigit_;

    std::unique_ptr<Sprite> game_;

    Vector2 spPos_;
    Vector2 spPos2_;
    Vector2 spPos3_;
    Vector2 spPos4_;
    Vector2 spPos5_;

    int enemyNum = 10;

    float t_ = 0.0f;

    bool isStart_ = false;
    bool isFinish_ = false;
};
