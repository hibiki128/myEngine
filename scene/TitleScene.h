#pragma once
#include "Audio.h"
#include "BaseScene.h"
#include "DebugCamera.h"
#include "Input.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "ParticleCommon.h"
#include "ParticleEmitter.h"
#include "SpriteCommon.h"
#include "WorldTransform.h"
#include "application/Base/BaseObject.h"
#include <application/Field/SkyDome/SkyDome.h>
#include <application/Field/Ground/Ground.h>
#include"Sprite.h"

class TitleScene : public BaseScene {
  public: // メンバ関数
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 終了
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画
    /// </summary>
    void Draw() override;

    /// <summary>
    /// オフスクリーン上に描画
    /// </summary>
    void DrawForOffScreen() override;

    ViewProjection *GetViewProjection() override { return &vp_; }

  private:
    void Debug();

    void CameraUpdate();

    void ChangeScene();

  private:
    Audio *audio_;
    Input *input_;
    Object3dCommon *objCommon_;
    SpriteCommon *spCommon_;
    ParticleCommon *ptCommon_;

    ViewProjection vp_;
    std::unique_ptr<DebugCamera> debugCamera_;

        // 天球
    std::unique_ptr<SkyDome> skyDome_;

    // 地面
    std::unique_ptr<Ground> ground_;

    std::unique_ptr<Sprite> title_;
    std::unique_ptr<Sprite> space_;
};
