#include "TitleScene.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "SrvManager.h"

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG
#include "line/DrawLine3D.h"
#include <LightGroup.h>

void TitleScene::Initialize() {
    audio_ = Audio::GetInstance();
    objCommon_ = Object3dCommon::GetInstance();
    spCommon_ = SpriteCommon::GetInstance();
    ptCommon_ = ParticleCommon::GetInstance();
    input_ = Input::GetInstance();
    vp_.Initialize();
    vp_.translation_ = {0.0f, 3.0f, -10.0f};

    debugCamera_ = std::make_unique<DebugCamera>();
    debugCamera_->Initialize(&vp_);

    skyDome_ = std::make_unique<SkyDome>();
    ground_ = std::make_unique<Ground>();

    skyDome_->Init("SkyDome");
    ground_->Init("Ground");

    title_ = std::make_unique<Sprite>();
    space_ = std::make_unique<Sprite>();

    title_->Initialize("game/title.png", {640.0f, 200.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
    space_->Initialize("game/space.png", {640.0f, 500.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
}

void TitleScene::Finalize() {
}

void TitleScene::Update() {
#ifdef _DEBUG
    // デバッグ
    Debug();
#endif // _DEBUG

    // カメラ更新
    CameraUpdate();

    // シーン切り替え
    ChangeScene();

    skyDome_->Update();
    ground_->Update();
}

void TitleScene::Draw() {
    /// -------描画処理開始-------

    /// Spriteの描画準備
    spCommon_->DrawCommonSetting();
    //-----Spriteの描画開始-----

    //------------------------

    objCommon_->DrawCommonSetting();
    //-----3DObjectの描画開始-----

    //--------------------------

    /// Particleの描画準備
    ptCommon_->DrawCommonSetting();
    //------Particleの描画開始-------

    //-----------------------------

    //-----線描画-----
#ifdef _DEBUG
    DrawLine3D::GetInstance()->Draw(vp_);
#endif // _DEBUG
    //---------------

    /// ----------------------------------

    /// -------描画処理終了-------
}

void TitleScene::DrawForOffScreen() {
    /// -------描画処理開始-------

    objCommon_->DrawCommonSetting();
    //-----3DObjectの描画開始-----
    skyDome_->Draw(vp_);
    ground_->Draw(vp_);
    //--------------------------

    /// Particleの描画準備
    ptCommon_->DrawCommonSetting();
    //------Particleの描画開始-------

    //-----------------------------

    /// Spriteの描画準備
    spCommon_->DrawCommonSetting();
    //-----Spriteの描画開始-----
    title_->Draw();
    space_->Draw();
    //------------------------

    /// ----------------------------------

    /// -------描画処理終了-------
}

void TitleScene::Debug() {
    ImGui::Begin("TitleScene:Debug");
    debugCamera_->imgui();
    LightGroup::GetInstance()->imgui();
    ImGui::End();
}

void TitleScene::CameraUpdate() {
    if (debugCamera_->GetActive()) {
        debugCamera_->Update();
    } else {
        vp_.UpdateMatrix();
    }
}

void TitleScene::ChangeScene() {
    if (input_->TriggerKey(DIK_SPACE)) {
        sceneManager_->NextSceneReservation("GAME");
    }
}
