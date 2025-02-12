#include "DemoScene.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "SrvManager.h"

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG
#include "line/DrawLine3D.h"
#include <LightGroup.h>

void DemoScene::Initialize() {
    audio_ = Audio::GetInstance();
    objCommon_ = Object3dCommon::GetInstance();
    spCommon_ = SpriteCommon::GetInstance();
    ptCommon_ = ParticleCommon::GetInstance();
    input_ = Input::GetInstance();
    vp_.Initialize();
    vp_.translation_ = {0.0f, 0.0f, -30.0f};

    debugCamera_ = std::make_unique<DebugCamera>();
    debugCamera_->Initialize(&vp_);

    emitter_ = std::make_unique<ParticleEmitter>();
    emitter_->Initialize();

    test_ = std::make_unique<BaseObject>();
    test_->Init("testObj");
    test_->CreateModel("debug/cube.obj");
    test_->CreateCollider();
}

void DemoScene::Finalize() {
}

void DemoScene::Update() {

#ifdef _DEBUG
    // デバッグ
    Debug();
#endif // _DEBUG

    // カメラ更新
    CameraUpdate();

    // シーン切り替え
    ChangeScene();

    test_->Update();
}

void DemoScene::Draw() {
    /// -------描画処理開始-------

    /// Spriteの描画準備
    spCommon_->DrawCommonSetting();
    //-----Spriteの描画開始-----

    //------------------------------

    objCommon_->DrawCommonSetting();
    //-----3DObjectの描画開始-----
    test_->Draw(vp_);
    //--------------------------

    /// Particleの描画準備
    ptCommon_->DrawCommonSetting();
    //------Particleの描画開始-------
    emitter_->Draw(vp_);
    //-----------------------------

    /// ----------------------------------

    /// -------描画処理終了-------
}

void DemoScene::DrawForOffScreen() {
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

    /// ----------------------------------

    /// -------描画処理終了-------
}

void DemoScene::Debug() {
    ImGui::Begin("DemoScene:Debug");
    debugCamera_->imgui();
    LightGroup::GetInstance()->imgui();
    ImGui::End();
    test_->DebugImGui();
    //emitter_->Debug();
}

void DemoScene::CameraUpdate() {
    if (debugCamera_->GetActive()) {
        debugCamera_->Update();
    } else {
        vp_.UpdateMatrix();
    }
}

void DemoScene::ChangeScene() {
}
