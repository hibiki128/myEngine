#include "ParticleEmitter.h"
#include "line/DrawLine3D.h"
#include "myEngine/Frame/Frame.h"
// コンストラクタ
ParticleEmitter::ParticleEmitter() {}

void ParticleEmitter::Initialize(std::string name) {
    if (!name.empty()) {
        name_ = name;
    }
    transform_.Initialize();
    emitFrequency_ = 0.1f;
    velocityMin_ = {-1.0f, -1.0f, -1.0f};
    velocityMax_ = {1.0f, 1.0f, 1.0f};
    lifeTimeMin_ = {1.0f};
    lifeTimeMax_ = {3.0f};
    isVisible_ = true;
    startAcce_ = {1.0f, 1.0f, 1.0f};
    endAcce_ = {1.0f, 1.0f, 1.0f};
    startScale_ = {1.0f, 1.0f, 1.0f};
    endScale_ = {1.0f, 1.0f, 1.0f};
    rotateVelocityMin_ = {-0.07f, -0.07f, -0.07f};
    rotateVelocityMax_ = {0.07f, 0.07f, 0.07f};
    count_ = 3;
    alphaMin_ = 1.0f;
    alphaMax_ = 1.0f;
    isBillBoard_ = false;
    isActive_ = true;
    isAcceMultiply_ = false;
    allScaleMin_ = {1.0f, 1.0f, 1.0f};
    allScaleMax_ = {1.0f, 1.0f, 1.0f};
    LoadFromJson();
    if (!name.empty()) {
        Manager_ = std::make_unique<ParticleManager>();
        Manager_->Initialize(SrvManager::GetInstance());
        Manager_->CreateParticleGroup(name_, fileName_);
        if (texturePath_.empty()) {
            texturePath_ = Manager_->GetTexturePath();
        }
        Manager_->SetTexture(texturePath_);
    }
}

// Update関数
void ParticleEmitter::Update() {
    // 経過時間を進める
    elapsedTime_ += Frame::DeltaTime();

    // 発生頻度に基づいてパーティクルを発生させる
    while (elapsedTime_ >= emitFrequency_) {
        Manager_->SetRandomRotate(isRandomRotate_);
        Manager_->SetAcceMultipy(isAcceMultiply_);
        Manager_->SetBillBorad(isBillBoard_);
        Manager_->SetRandomSize(isRandomScale_);
        Manager_->SetAllRandomSize(isAllRamdomScale_);
        Manager_->SetSinMove(isSinMove_);
        Manager_->SetFaceDirection(isFaceDirection_);
        Emit();                         // パーティクルを発生させる
        elapsedTime_ -= emitFrequency_; // 過剰に進んだ時間を考慮
    }
}

void ParticleEmitter::UpdateOnce() {
    isActive_ = false;
    if (!isActive_) {
        Manager_->SetRandomRotate(isRandomRotate_);
        Manager_->SetAcceMultipy(isAcceMultiply_);
        Manager_->SetBillBorad(isBillBoard_);
        Manager_->SetRandomSize(isRandomScale_);
        Manager_->SetAllRandomSize(isAllRamdomScale_);
        Manager_->SetSinMove(isSinMove_);
        Manager_->SetFaceDirection(isFaceDirection_);
        Emit(); // パーティクルを発生させる
        isActive_ = true;
    }
}

void ParticleEmitter::Draw(const ViewProjection &vp_) {
    transform_.UpdateMatrix();
    if (Manager_) {
        Manager_->Update(vp_);
        Manager_->Draw();
    }
    DrawEmitter();
}

void ParticleEmitter::DrawEmitter() {
    // isVisibleがtrueのときだけ描画
    if (!isVisible_)
        return;

    // 立方体のローカル座標での基本頂点（スケーリング済み）
    std::array<Vector3, 8> localVertices = {
        Vector3{-1.0f, -1.0f, -1.0f}, // 左下手前
        Vector3{1.0f, -1.0f, -1.0f},  // 右下手前
        Vector3{-1.0f, 1.0f, -1.0f},  // 左上手前
        Vector3{1.0f, 1.0f, -1.0f},   // 右上手前
        Vector3{-1.0f, -1.0f, 1.0f},  // 左下奥
        Vector3{1.0f, -1.0f, 1.0f},   // 右下奥
        Vector3{-1.0f, 1.0f, 1.0f},   // 左上奥
        Vector3{1.0f, 1.0f, 1.0f}     // 右上奥
    };

    // ワールド変換結果を格納する配列
    std::array<Vector3, 8> worldVertices;

    // ワールド行列の計算
    Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale_, transform_.rotation_, transform_.translation_);

    // 頂点のワールド変換
    for (size_t i = 0; i < localVertices.size(); i++) {
        worldVertices[i] = Transformation(localVertices[i], worldMatrix);
    }

    // エッジリスト（線の接続順）
    constexpr std::array<std::pair<int, int>, 12> edges = {
        std::make_pair(0, 1), std::make_pair(1, 3), std::make_pair(3, 2), std::make_pair(2, 0), // 前面
        std::make_pair(4, 5), std::make_pair(5, 7), std::make_pair(7, 6), std::make_pair(6, 4), // 背面
        std::make_pair(0, 4), std::make_pair(1, 5), std::make_pair(2, 6), std::make_pair(3, 7)  // 側面
    };

    // エッジ描画
    for (const auto &edge : edges) {
        DrawLine3D::GetInstance()->SetPoints(worldVertices[edge.first], worldVertices[edge.second]);
    }
}

void ParticleEmitter::SetTexture(const std::string &filePath) {
    texturePath_ = filePath;
    Manager_->SetTexture(filePath);
}

// Emit関数
void ParticleEmitter::Emit() {
    // ParticleManagerのEmit関数を呼び出す
    Manager_->Emit(
        name_,
        transform_.translation_,
        count_,
        transform_.scale_, // スケールを引数として渡す
        velocityMin_,      // 最小速度を引数として渡す
        velocityMax_,      // 最大速度を引数として渡す
        lifeTimeMin_,      // 最小ライフタイムを引数として渡す
        lifeTimeMax_,      // 最大ライフタイムを引数として渡す
        startScale_,
        endScale_,
        startAcce_,
        endAcce_,
        startRote_,
        endRote_,
        isRandomColor_,
        alphaMin_,
        alphaMax_,
        rotateVelocityMin_,
        rotateVelocityMax_,
        allScaleMax_,
        allScaleMin_,
        scaleMin_,
        scaleMax_,
        transform_.rotation_);
}

void ParticleEmitter::CreateParticle(const std::string &name, const std::string &fileName, const std::string &texturePath) {

    name_ = name;
    fileName_ = fileName;
    Manager_ = std::make_unique<ParticleManager>();
    Manager_->Initialize(SrvManager::GetInstance());
    Manager_->CreateParticleGroup(name_, fileName_);
    if (texturePath_.empty()) {
        texturePath_ = Manager_->GetTexturePath();
    }
    texturePath_ = texturePath;
    Manager_->SetTexture(texturePath_);
}

#pragma region ImGui関連

void ParticleEmitter::SaveToJson() {
    json j;

    // メンバー変数をJSONに保存
    j["emitter"]["translate"] = {transform_.translation_.x, transform_.translation_.y, transform_.translation_.z};
    j["emitter"]["rotation"] = {transform_.rotation_.x, transform_.rotation_.y, transform_.rotation_.z};
    j["emitter"]["scale"] = {transform_.scale_.x, transform_.scale_.y, transform_.scale_.z};
    j["count"] = count_;
    j["emitFrequency"] = emitFrequency_;
    j["lifeTimeMin"] = lifeTimeMin_;
    j["lifeTimeMax"] = lifeTimeMax_;
    j["alphaMin"] = alphaMin_;
    j["alphaMax"] = alphaMax_;
    j["scaleMin"] = scaleMin_;
    j["scaleMax"] = scaleMax_;
    j["velocityMin"] = {velocityMin_.x, velocityMin_.y, velocityMin_.z};
    j["velocityMax"] = {velocityMax_.x, velocityMax_.y, velocityMax_.z};
    j["startScale"] = {startScale_.x, startScale_.y, startScale_.z};
    j["endScale"] = {endScale_.x, endScale_.y, endScale_.z};
    j["startAcce"] = {startAcce_.x, startAcce_.y, startAcce_.z};
    j["endAcce"] = {endAcce_.x, endAcce_.y, endAcce_.z};
    j["startRote"] = {startRote_.x, startRote_.y, startRote_.z};
    j["endRote"] = {endRote_.x, endRote_.y, endRote_.z};
    j["rotateVelocityMin"] = {rotateVelocityMin_.x, rotateVelocityMin_.y, rotateVelocityMin_.z};
    j["rotateVelocityMax"] = {rotateVelocityMax_.x, rotateVelocityMax_.y, rotateVelocityMax_.z};
    j["allScaleMin"] = {allScaleMin_.x, allScaleMin_.y, allScaleMin_.z};
    j["allScaleMax"] = {allScaleMax_.x, allScaleMax_.y, allScaleMax_.z};
    j["isRandomScale"] = isRandomScale_;
    j["isAllRamdomScale"] = isAllRamdomScale_;
    j["isRandomColor"] = isRandomColor_;
    j["isRandomRotate"] = isRandomRotate_;
    j["isVisible"] = isVisible_;
    j["isBillBoard"] = isBillBoard_;
    j["isActive"] = isActive_;
    j["isAcceMultiply"] = isAcceMultiply_;
    j["isSinMove"] = isSinMove_;
    j["isFaceDirection"] = isFaceDirection_;
    j["ObjectName"] = fileName_;
    j["TexturePath"] = texturePath_;

    // ディレクトリを作成し、JSONファイルを保存
    std::filesystem::create_directories("resources/jsons/Particle/");
    std::ofstream outFile("resources/jsons/Particle/" + name_ + ".json");
    outFile << j.dump(4);
}

void ParticleEmitter::LoadFromJson() {
    std::ifstream inFile("resources/jsons/Particle/" + name_ + ".json");
    if (!inFile.is_open()) {
        return; // JSONファイルがない場合は早期リターン
    }

    json j;
    inFile >> j;

    // emitter内のtransform情報を読み込み
    transform_.translation_.x = j["emitter"]["translate"][0];
    transform_.translation_.y = j["emitter"]["translate"][1];
    transform_.translation_.z = j["emitter"]["translate"][2];

    transform_.rotation_.x = j["emitter"]["rotation"][0];
    transform_.rotation_.y = j["emitter"]["rotation"][1];
    transform_.rotation_.z = j["emitter"]["rotation"][2];

    transform_.scale_.x = j["emitter"]["scale"][0];
    transform_.scale_.y = j["emitter"]["scale"][1];
    transform_.scale_.z = j["emitter"]["scale"][2];

    // その他の変数の読み込み
    count_ = j["count"];
    emitFrequency_ = j["emitFrequency"];
    lifeTimeMin_ = j["lifeTimeMin"];
    lifeTimeMax_ = j["lifeTimeMax"];
    alphaMin_ = j["alphaMin"];
    alphaMax_ = j["alphaMax"];
    scaleMin_ = j["scaleMin"];
    scaleMax_ = j["scaleMax"];

    velocityMin_.x = j["velocityMin"][0];
    velocityMin_.y = j["velocityMin"][1];
    velocityMin_.z = j["velocityMin"][2];

    velocityMax_.x = j["velocityMax"][0];
    velocityMax_.y = j["velocityMax"][1];
    velocityMax_.z = j["velocityMax"][2];

    startScale_.x = j["startScale"][0];
    startScale_.y = j["startScale"][1];
    startScale_.z = j["startScale"][2];

    endScale_.x = j["endScale"][0];
    endScale_.y = j["endScale"][1];
    endScale_.z = j["endScale"][2];

    startAcce_.x = j["startAcce"][0];
    startAcce_.y = j["startAcce"][1];
    startAcce_.z = j["startAcce"][2];

    endAcce_.x = j["endAcce"][0];
    endAcce_.y = j["endAcce"][1];
    endAcce_.z = j["endAcce"][2];

    startRote_.x = j["startRote"][0];
    startRote_.y = j["startRote"][1];
    startRote_.z = j["startRote"][2];

    endRote_.x = j["endRote"][0];
    endRote_.y = j["endRote"][1];
    endRote_.z = j["endRote"][2];

    rotateVelocityMin_.x = j["rotateVelocityMin"][0];
    rotateVelocityMin_.y = j["rotateVelocityMin"][1];
    rotateVelocityMin_.z = j["rotateVelocityMin"][2];

    rotateVelocityMax_.x = j["rotateVelocityMax"][0];
    rotateVelocityMax_.y = j["rotateVelocityMax"][1];
    rotateVelocityMax_.z = j["rotateVelocityMax"][2];

    allScaleMin_.x = j["allScaleMin"][0];
    allScaleMin_.y = j["allScaleMin"][1];
    allScaleMin_.z = j["allScaleMin"][2];

    allScaleMax_.x = j["allScaleMax"][0];
    allScaleMax_.y = j["allScaleMax"][1];
    allScaleMax_.z = j["allScaleMax"][2];

    isRandomScale_ = j["isRandomScale"];
    isAllRamdomScale_ = j["isAllRamdomScale"];
    isRandomColor_ = j["isRandomColor"];
    isRandomRotate_ = j["isRandomRotate"];
    isVisible_ = j["isVisible"];
    isBillBoard_ = j["isBillBoard"];
    isActive_ = j["isActive"];
    isAcceMultiply_ = j["isAcceMultiply"];
    isSinMove_ = j["isSinMove"];
    isFaceDirection_ = j["isFaceDirection"];
    fileName_ = j["ObjectName"];
    texturePath_ = j["TexturePath"];
}

void ParticleEmitter::ShowImGuiEditor() {
    if (ImGui::BeginTabBar("パーティクル")) {
        if (ImGui::BeginTabItem("パーティクル作成")) {

            // 名前の入力
            char nameBuffer[256];
            strcpy_s(nameBuffer, sizeof(nameBuffer), localName_.c_str());
            if (ImGui::InputText("パーティクルの名前", nameBuffer, sizeof(nameBuffer))) {
                localName_ = std::string(nameBuffer);
            }

            // モデルとテクスチャの処理を分ける
            ImGui::Spacing();

            // モデル選択アイテム
            if (ImGui::CollapsingHeader("モデル")) {
                // モデルファイル選択
                static std::filesystem::path baseDirObj = "resources/models/";
                static std::filesystem::path currentDirObj = "resources/models";
                static std::string selectedFolderObj = "";
                static std::string selectedFileObj = "";

                // 「戻る」ボタン（上の階層に戻る）
                if (currentDirObj != "resources/models") {
                    if (ImGui::Button("< 戻る(Model)")) {
                        currentDirObj = currentDirObj.parent_path();
                        selectedFolderObj = "";
                        selectedFileObj = "";
                    }
                }

                // フォルダ一覧
                std::vector<std::string> foldersObj;
                std::vector<std::string> objFiles;

                for (const auto &entry : std::filesystem::directory_iterator(currentDirObj)) {
                    if (entry.is_directory()) {
                        foldersObj.push_back(entry.path().filename().string());
                    } else if (entry.path().extension() == ".obj") {
                        objFiles.push_back(entry.path().filename().string());
                    }
                }

                // フォルダ選択 (クリックで移動)
                if (!foldersObj.empty()) {
                    ImGui::Text("フォルダ");
                    ImGui::Separator();
                    for (const auto &folder : foldersObj) {
                        std::string folderNameTex = folder + " (Model)"; // フォルダ名に "(Tex)" を追加
                        if (ImGui::Selectable(folderNameTex.c_str(), selectedFolderObj == folder)) {
                            selectedFolderObj = folderNameTex;
                            currentDirObj = currentDirObj / folder; // フォルダ移動
                            selectedFileObj = "";                   // 新しいフォルダを開いたらファイル選択をリセット
                        }
                        ImGui::Separator();
                    }
                }

                // `.obj` ファイル選択
                if (!objFiles.empty()) {
                    ImGui::Text("モデルファイル:");
                    if (ImGui::BeginCombo("ファイル選択", selectedFileObj.empty() ? "なし" : selectedFileObj.c_str())) {
                        for (const auto &file : objFiles) {
                            bool isSelected = (file == selectedFileObj);
                            if (ImGui::Selectable(file.c_str(), isSelected)) {
                                selectedFileObj = file;

                                // `baseDirObj` からの相対パスを取得
                                std::filesystem::path relativePath = (currentDirObj / file).lexically_relative(baseDirObj);

                                // Windowsのバックスラッシュをスラッシュに変換
                                std::string pathStr = relativePath.string();
                                std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

                                // `fileNameObj_` に保存
                                localFileObj_ = pathStr;
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }
            }

            // テクスチャ選択アイテム
            if (ImGui::CollapsingHeader("テクスチャ")) {
                // テクスチャファイル選択
                static std::filesystem::path baseDirTex = "resources/images/";
                static std::filesystem::path currentDirTex = "resources/images";
                static std::string selectedFolderTex = "";
                static std::string selectedFileTex = "";

                // 「戻る」ボタン（テクスチャ用）
                if (currentDirTex != "resources/images") {
                    if (ImGui::Button("< 戻る(Tex)")) {
                        currentDirTex = currentDirTex.parent_path();
                        selectedFolderTex = "";
                        selectedFileTex = "";
                    }
                }

                // フォルダ一覧
                std::vector<std::string> foldersTex;
                std::vector<std::string> texFiles;

                for (const auto &entry : std::filesystem::directory_iterator(currentDirTex)) {
                    if (entry.is_directory()) {
                        foldersTex.push_back(entry.path().filename().string());
                    } else if (entry.path().extension() == ".png" || entry.path().extension() == ".jpg") {
                        texFiles.push_back(entry.path().filename().string());
                    }
                }

                // フォルダ選択 (クリックで移動)
                if (!foldersTex.empty()) {
                    ImGui::Text("フォルダ");
                    ImGui::Separator();
                    for (auto &folder : foldersTex) {
                        std::string folderNameTex = folder + " (Tex)"; // フォルダ名に "(Tex)" を追加
                        if (ImGui::Selectable(folderNameTex.c_str(), selectedFolderTex == folder)) {
                            selectedFolderTex = folderNameTex;
                            currentDirTex = currentDirTex / folder; // フォルダ移動
                            selectedFileTex = "";                   // 新しいフォルダを開いたらファイル選択をリセット
                        }
                        ImGui::Separator();
                    }
                }

                // `.png`, `.jpg` テクスチャファイル選択
                if (!texFiles.empty()) {
                    ImGui::Text("テクスチャファイル:");
                    if (ImGui::BeginCombo("ファイル選択 ", selectedFileTex.empty() ? "なし" : selectedFileTex.c_str())) {
                        for (const auto &file : texFiles) {
                            bool isSelected = (file == selectedFileTex);
                            if (ImGui::Selectable(file.c_str(), isSelected)) {
                                selectedFileTex = file;

                                // `baseDirTex` からの相対パスを取得
                                std::filesystem::path relativePath = (currentDirTex / file).lexically_relative(baseDirTex);

                                // Windowsのバックスラッシュをスラッシュに変換
                                std::string pathStr = relativePath.string();
                                std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

                                // `texturePath_` に保存
                                localTexturePath_ = pathStr;
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }
            }

            // パーティクル作成ボタン
            ImGui::Spacing();
            if (!localName_.empty() && !localFileObj_.empty() && !localTexturePath_.empty()) {
                if (ImGui::Button("パーティクル生成")) {
                    CreateParticle(localName_, localFileObj_, localTexturePath_);
                    localName_.clear();
                    localFileObj_.clear();
                    localTexturePath_.clear(); // テクスチャのパスもクリア
                }
            }

               if (ImGui::CollapsingHeader("パーティクルデータのロード")) {
                ShowFileSelector();
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void ParticleEmitter::DebugParticleData() {

    if (ImGui::BeginTabBar("パーティクル")) {
        if (ImGui::BeginTabItem(name_.c_str())) {

            // 基本データセクション
            if (ImGui::CollapsingHeader("エミッターデータ")) {
                // トランスフォームデータをフレーム内に配置
                ImGui::Text("Transformデータ:");
                ImGui::Separator();
                ImGui::Columns(2, "TransformColumns", false); // 2列レイアウト
                ImGui::Text("位置");
                ImGui::NextColumn();
                ImGui::DragFloat3("##位置", &transform_.translation_.x, 0.1f);
                ImGui::NextColumn();
                ImGui::Text("回転");
                ImGui::NextColumn();
                float rotationDegrees[3] = {
                    radiansToDegrees(transform_.rotation_.x),
                    radiansToDegrees(transform_.rotation_.y),
                    radiansToDegrees(transform_.rotation_.z)};

                // ドラッグUIを使用し、度数法で値を操作
                if (ImGui::DragFloat3("##回転 (度)", rotationDegrees, 0.1f, -360.0f, 360.0f)) {
                    // 操作後、度数法からラジアンに戻して保存
                    transform_.rotation_.x = degreesToRadians(rotationDegrees[0]);
                    transform_.rotation_.y = degreesToRadians(rotationDegrees[1]);
                    transform_.rotation_.z = degreesToRadians(rotationDegrees[2]);
                }

                ImGui::NextColumn();
                ImGui::Text("大きさ");
                ImGui::NextColumn();
                ImGui::DragFloat3("##大きさ", &transform_.scale_.x, 0.1f, 0.0f);
                ImGui::Columns(1); // 列終了
                ImGui::Separator();

                // 可視性フラグ
                ImGui::Checkbox("表示", &isVisible_);
            }

            // パーティクルデータセクション
            if (ImGui::CollapsingHeader("パーティクルデータ")) {
                // LifeTimeを折りたたみ可能にする
                if (ImGui::TreeNode("寿命")) {
                    ImGui::Text("寿命設定:");
                    ImGui::Separator();
                    ImGui::DragFloat("最大値", &lifeTimeMax_, 0.1f, 0.0f);
                    ImGui::DragFloat("最小値", &lifeTimeMin_, 0.1f, 0.0f);
                    lifeTimeMin_ = std::clamp(lifeTimeMin_, 0.0f, lifeTimeMax_);
                    lifeTimeMax_ = std::clamp(lifeTimeMax_, lifeTimeMin_, 10.0f);
                    ImGui::TreePop();
                }

                ImGui::Separator();

                // 速度と加速度
                if (ImGui::TreeNode("速度、加速度")) {
                    ImGui::Text("速度:");
                    ImGui::DragFloat3("最大値", &velocityMax_.x, 0.1f);
                    ImGui::DragFloat3("最小値", &velocityMin_.x, 0.1f);
                    velocityMin_.x = std::clamp(velocityMin_.x, -FLT_MAX, velocityMax_.x);
                    velocityMax_.x = std::clamp(velocityMax_.x, velocityMin_.x, FLT_MAX);
                    velocityMin_.y = std::clamp(velocityMin_.y, -FLT_MAX, velocityMax_.y);
                    velocityMax_.y = std::clamp(velocityMax_.y, velocityMin_.y, FLT_MAX);
                    velocityMin_.z = std::clamp(velocityMin_.z, -FLT_MAX, velocityMax_.z);
                    velocityMax_.z = std::clamp(velocityMax_.z, velocityMin_.z, FLT_MAX);
                    ImGui::Text("加速度:");
                    ImGui::DragFloat3("最初", &startAcce_.x, 0.001f);
                    ImGui::DragFloat3("最後", &endAcce_.x, 0.001f);
                    ImGui::Checkbox("乗算", &isAcceMultiply_);
                    ImGui::TreePop();
                }

                ImGui::Separator();

                // サイズ
                if (ImGui::TreeNode("大きさ")) {
                    ImGui::Text("大きさ:");
                    if (isAllRamdomScale_) {
                        ImGui::DragFloat3("最大値", &allScaleMax_.x, 0.1f, 0.0f);
                        ImGui::DragFloat3("最小値", &allScaleMin_.x, 0.1f, 0.0f);
                        allScaleMin_.x = std::clamp(allScaleMin_.x, -FLT_MAX, allScaleMax_.x);
                        allScaleMax_.x = std::clamp(allScaleMax_.x, allScaleMin_.x, FLT_MAX);
                        allScaleMin_.y = std::clamp(allScaleMin_.y, -FLT_MAX, allScaleMax_.y);
                        allScaleMax_.y = std::clamp(allScaleMax_.y, allScaleMin_.y, FLT_MAX);
                        allScaleMin_.z = std::clamp(allScaleMin_.z, -FLT_MAX, allScaleMax_.z);
                        allScaleMax_.z = std::clamp(allScaleMax_.z, allScaleMin_.z, FLT_MAX);
                    } else if (isRandomScale_) {
                        ImGui::DragFloat("最大値", &scaleMax_, 0.1f, 0.0f);
                        ImGui::DragFloat("最小値", &scaleMin_, 0.1f, 0.0f);
                        scaleMax_ = std::clamp(scaleMax_, scaleMin_, FLT_MAX);
                        scaleMin_ = std::clamp(scaleMin_, 0.0f, scaleMax_);
                    } else if (isSinMove_) {
                        ImGui::DragFloat3("最初", &startScale_.x, 0.1f, 0.0f);
                    } else {
                        ImGui::DragFloat3("最初", &startScale_.x, 0.1f, 0.0f);
                    }
                    if (!isSinMove_) {
                        ImGui::DragFloat3("最後", &endScale_.x, 0.1f);
                    }
                    ImGui::Checkbox("均等にランダムな大きさ", &isRandomScale_);
                    ImGui::Checkbox("ばらばらにランダムな大きさ", &isAllRamdomScale_);
                    ImGui::Checkbox("sin波の動き", &isSinMove_);
                    ImGui::TreePop();
                }

                ImGui::Separator();

                // 回転
                if (ImGui::TreeNode("回転")) {
                    if (!isRandomRotate_) {
                        float startRotationDegrees[3] = {
                            radiansToDegrees(startRote_.x),
                            radiansToDegrees(startRote_.y),
                            radiansToDegrees(startRote_.z)};
                        float endRotationDegrees[3] = {
                            radiansToDegrees(endRote_.x),
                            radiansToDegrees(endRote_.y),
                            radiansToDegrees(endRote_.z)};
                        if (ImGui::DragFloat3("最初", startRotationDegrees, 0.1f)) {
                            startRote_.x = degreesToRadians(startRotationDegrees[0]);
                            startRote_.y = degreesToRadians(startRotationDegrees[1]);
                            startRote_.z = degreesToRadians(startRotationDegrees[2]);
                        }
                        if (ImGui::DragFloat3("最後", endRotationDegrees, 0.1f)) {
                            endRote_.x = degreesToRadians(endRotationDegrees[0]);
                            endRote_.y = degreesToRadians(endRotationDegrees[1]);
                            endRote_.z = degreesToRadians(endRotationDegrees[2]);
                        }
                    }
                    if (isRandomRotate_) {
                        ImGui::DragFloat3("最大値", &rotateVelocityMax_.x, 0.01f);
                        ImGui::DragFloat3("最小値", &rotateVelocityMin_.x, 0.01f);
                        rotateVelocityMin_.x = std::clamp(rotateVelocityMin_.x, -FLT_MAX, rotateVelocityMax_.x);
                        rotateVelocityMax_.x = std::clamp(rotateVelocityMax_.x, rotateVelocityMin_.x, FLT_MAX);
                        rotateVelocityMin_.y = std::clamp(rotateVelocityMin_.y, -FLT_MAX, rotateVelocityMax_.y);
                        rotateVelocityMax_.y = std::clamp(rotateVelocityMax_.y, rotateVelocityMin_.y, FLT_MAX);
                        rotateVelocityMin_.z = std::clamp(rotateVelocityMin_.z, -FLT_MAX, rotateVelocityMax_.z);
                        rotateVelocityMax_.z = std::clamp(rotateVelocityMax_.z, rotateVelocityMin_.z, FLT_MAX);
                    }
                    ImGui::Checkbox("ランダムな回転", &isRandomRotate_);
                    ImGui::Checkbox("進行方向に向ける", &isFaceDirection_);
                    ImGui::TreePop();
                }

                ImGui::Separator();

                // Alphaを折りたたみ可能にする
                if (ImGui::TreeNode("透明度")) {
                    ImGui::Text("透明度の設定:");
                    ImGui::DragFloat("最大値", &alphaMax_, 0.01f, 0.0f, 1.0f);
                    ImGui::DragFloat("最小値", &alphaMin_, 0.01f, 0.0f, 1.0f);
                    alphaMin_ = std::clamp(alphaMin_, 0.0f, alphaMax_);
                    alphaMax_ = std::clamp(alphaMax_, alphaMin_, 1.0f);
                    ImGui::TreePop();
                }
            }

            // エミット設定セクション
            if (ImGui::CollapsingHeader("パーティクルの数、間隔")) {
                ImGui::DragFloat("間隔", &emitFrequency_, 0.001f, 0.001f, 100.0f);
                ImGui::InputInt("数", &count_, 1, 100);
                count_ = std::clamp(count_, 0, 10000);
            }

            // その他の設定セクション
            if (ImGui::CollapsingHeader("各状態の設定")) {
                ImGui::Checkbox("ビルボード", &isBillBoard_);
                ImGui::Checkbox("ランダムカラー", &isRandomColor_);
            }

            if (ImGui::Button("セーブ")) {
                SaveToJson();
                std::string message = std::format("ParticleData saved.");
                MessageBoxA(nullptr, message.c_str(), "Particle", 0);
            }

            ImGui::Checkbox("自動生成", &isAuto_);
            if (isAuto_) {
                Update();
            }

            if (ImGui::Button("生成")) {
                UpdateOnce();
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    };
}

std::vector<std::string> ParticleEmitter::GetJsonFiles() {
    static std::vector<std::string> jsonFiles; // キャッシュされたJSONファイルリスト
    static size_t lastFileCount = 0;           // 最後に取得したJSONファイル数
    std::filesystem::path baseDir = "resources/jsons/Particle";

    // ディレクトリが存在しない場合はキャッシュをクリア
    if (!std::filesystem::exists(baseDir) || !std::filesystem::is_directory(baseDir)) {
        jsonFiles.clear();
        lastFileCount = 0;
        return jsonFiles;
    }

    // 現在のファイル数をカウント
    size_t currentFileCount = std::count_if(
        std::filesystem::directory_iterator(baseDir),
        std::filesystem::directory_iterator{},
        [](const std::filesystem::directory_entry &entry) {
            return entry.path().extension() == ".json";
        });

    // ファイル数が変わった場合のみ更新
    if (currentFileCount != lastFileCount) {
        jsonFiles.clear(); // リストをクリア
        for (const auto &entry : std::filesystem::directory_iterator(baseDir)) {
            if (entry.path().extension() == ".json") {
                jsonFiles.push_back(entry.path().filename().string());
            }
        }
        lastFileCount = currentFileCount; // 更新したファイル数を記録
    }

    return jsonFiles;
}

void ParticleEmitter::ShowFileSelector() {
    static int selectedIndex = -1;
    std::vector<std::string> jsonFiles = GetJsonFiles();

    // JSONファイルがない場合のチェック
    if (jsonFiles.empty()) {
        ImGui::Text("Jsonファイルが見つかりませんでした");
        return;
    }

    // ファイルリストをCスタイル文字列の配列に変換
    std::vector<const char *> fileNames;
    for (const auto &filePath : jsonFiles) {
        fileNames.push_back(filePath.c_str());
    }

    ImGui::Text("Jsonファイルの選択:");
    ImGui::Separator();

    // Comboボックスでファイル選択
    if (ImGui::Combo("JSON Files", &selectedIndex, fileNames.data(), static_cast<int>(fileNames.size()))) {
        // ファイル選択時の動作（選択されたファイル名を表示）
        if (selectedIndex >= 0) {
            ImGui::Text("ファイル選択:");
            ImGui::TextWrapped("%s", jsonFiles[selectedIndex].c_str());
        }
    }

    // ボタンでパーティクルデータをセット
    if (selectedIndex >= 0 && ImGui::Button("パーティクルデータのセット")) {
        // name_ に ".json" を除いた名前を設定
        std::string selectedFileName = jsonFiles[selectedIndex];
        name_ = selectedFileName.substr(0, selectedFileName.find_last_of('.')); // ".json" を除去
        Initialize();
        CreateParticle(name_, fileName_, texturePath_);
    }
}

void ParticleEmitter::LoadFromJson(const std::string &name) {
    std::ifstream inFile("resources/jsons/Particle/" + name + ".json");
    if (!inFile.is_open()) {
        return; // JSONファイルがない場合は早期リターン
    }

    json j;
    inFile >> j;

    // emitter内のtransform情報を読み込み
    transform_.translation_.x = j["emitter"]["translate"][0];
    transform_.translation_.y = j["emitter"]["translate"][1];
    transform_.translation_.z = j["emitter"]["translate"][2];

    transform_.rotation_.x = j["emitter"]["rotation"][0];
    transform_.rotation_.y = j["emitter"]["rotation"][1];
    transform_.rotation_.z = j["emitter"]["rotation"][2];

    transform_.scale_.x = j["emitter"]["scale"][0];
    transform_.scale_.y = j["emitter"]["scale"][1];
    transform_.scale_.z = j["emitter"]["scale"][2];

    // その他の変数の読み込み
    count_ = j["count"];
    emitFrequency_ = j["emitFrequency"];
    lifeTimeMin_ = j["lifeTimeMin"];
    lifeTimeMax_ = j["lifeTimeMax"];
    alphaMin_ = j["alphaMin"];
    alphaMax_ = j["alphaMax"];
    scaleMin_ = j["scaleMin"];
    scaleMax_ = j["scaleMax"];

    velocityMin_.x = j["velocityMin"][0];
    velocityMin_.y = j["velocityMin"][1];
    velocityMin_.z = j["velocityMin"][2];

    velocityMax_.x = j["velocityMax"][0];
    velocityMax_.y = j["velocityMax"][1];
    velocityMax_.z = j["velocityMax"][2];

    startScale_.x = j["startScale"][0];
    startScale_.y = j["startScale"][1];
    startScale_.z = j["startScale"][2];

    endScale_.x = j["endScale"][0];
    endScale_.y = j["endScale"][1];
    endScale_.z = j["endScale"][2];

    startAcce_.x = j["startAcce"][0];
    startAcce_.y = j["startAcce"][1];
    startAcce_.z = j["startAcce"][2];

    endAcce_.x = j["endAcce"][0];
    endAcce_.y = j["endAcce"][1];
    endAcce_.z = j["endAcce"][2];

    startRote_.x = j["startRote"][0];
    startRote_.y = j["startRote"][1];
    startRote_.z = j["startRote"][2];

    endRote_.x = j["endRote"][0];
    endRote_.y = j["endRote"][1];
    endRote_.z = j["endRote"][2];

    rotateVelocityMin_.x = j["rotateVelocityMin"][0];
    rotateVelocityMin_.y = j["rotateVelocityMin"][1];
    rotateVelocityMin_.z = j["rotateVelocityMin"][2];

    rotateVelocityMax_.x = j["rotateVelocityMax"][0];
    rotateVelocityMax_.y = j["rotateVelocityMax"][1];
    rotateVelocityMax_.z = j["rotateVelocityMax"][2];

    allScaleMin_.x = j["allScaleMin"][0];
    allScaleMin_.y = j["allScaleMin"][1];
    allScaleMin_.z = j["allScaleMin"][2];

    allScaleMax_.x = j["allScaleMax"][0];
    allScaleMax_.y = j["allScaleMax"][1];
    allScaleMax_.z = j["allScaleMax"][2];

    isRandomScale_ = j["isRandomScale"];
    isAllRamdomScale_ = j["isAllRamdomScale"];
    isRandomColor_ = j["isRandomColor"];
    isRandomRotate_ = j["isRandomRotate"];
    isVisible_ = j["isVisible"];
    isBillBoard_ = j["isBillBoard"];
    isActive_ = j["isActive"];
    isAcceMultiply_ = j["isAcceMultiply"];
    isSinMove_ = j["isSinMove"];
    isFaceDirection_ = j["isFaceDirection"];
}

// ImGuiで値を動かす関数
void ParticleEmitter::Debug() {
#ifdef _DEBUG
    if (ImGui::Begin("Particle Emitter Editor")) {
        ShowImGuiEditor();
        if (!name_.empty() && Manager_) {
            DebugParticleData();
        }
        ImGui::End();
    }
#endif
}
#pragma endregion