#include "Player.h"
#include <algorithm>
#include <cassert>
#include <imgui.h>
#include"application/camera/RailCamera.h"

Player::~Player() {

}

void Player::Initilaize(ViewProjection* viewProjection, const Vector3& position) {
	// 引数として受け取ったデータをメンバ変数に記録する
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("debug/suzannu.obj");

	reticle_ = std::make_unique<Sprite>();
	reticle_->Initialize("Reticle.png", { 640.0f,360.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f });

	reticleObj_ = std::make_unique<Object3d>();
	reticleObj_->Initialize("debug/ICO.obj");

	spritePos = reticle_->GetPosition();
	// 正投影のプロジェクション行列を設定
	viewProjection->matProjection_ = MakeOrthographicMatrix(
		-WinApp::kClientWidth / 2.0f, WinApp::kClientWidth / 2.0f,   // 左右
		-WinApp::kClientHeight / 2.0f, WinApp::kClientHeight / 2.0f, // 上下
		0.1f, 1000.0f                                                // 近クリップ面と遠クリップ面のZ値
	);
	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = { 0.3f,0.3f,0.3f };

	Reticle3Dwt_.Initialize();
	Reticle3Dwt_.scale_ = { 1.0f,1.0f,1.0f };

	LeftBullet_ = std::make_unique<playerBullet>();
	LeftBulletOffset = { -5.0f, -2.5f, -4.5f };
	LeftBullet_->Initialize();
	RightBullet_ = std::make_unique<playerBullet>();
	RightBulletOffset = { 5.0f, -2.5f, -4.5f };
	RightBullet_->Initialize();
	CenterBullet_ = std::make_unique<playerBullet>();
	CenterBulletOffset = { 0.0f, -2.5f, -4.5f };
	CenterBullet_->Initialize();
	LeftRotateOffset = { 0.0f,0.0f,0.0f };
	RightRotateOffset = { 0.0f,0.0f,0.0f };
	CenterRotateOffset = { 0.0f,0.0f,0.0f };

	LeftBullet_->SetPlayer(this);
	RightBullet_->SetPlayer(this);
	CenterBullet_->SetPlayer(this);

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void Player::Update() {

	// 攻撃処理
	Attack();

	TramsformReticle();

	MoveAim();

	// 3Dレティクルの方向にbullet_を向ける
	AimBulletAtReticle();

	// 行列更新
	worldTransform_.UpdateMatrix();

	LeftBullet_->SetPosition(LeftBulletOffset);
	//LeftBullet_->SetRotation(LeftRotateOffset);
	LeftBullet_->Update();
	RightBullet_->SetPosition(RightBulletOffset);
	//RightBullet_->SetRotation(RightRotateOffset);
	RightBullet_->Update();
	CenterBullet_->SetPosition(CenterBulletOffset);
	//CenterBullet_->SetRotation(CenterRotateOffset);
	CenterBullet_->Update();

}

void Player::Draw() {
	obj_->Draw(worldTransform_, *viewProjection_);
	reticleObj_->Draw(Reticle3Dwt_, *viewProjection_);
	LeftBullet_->Draw(*viewProjection_);
	RightBullet_->Draw(*viewProjection_);
}

void Player::DrawUI()
{
	reticle_->Draw();
}

void Player::imgui() {
	// キャラクターの座標を画面表示する処理
	if (ImGui::BeginTabBar("player")) {
		if (ImGui::BeginTabItem("Player")) {
			ImGui::DragFloat3("position", &worldTransform_.translation_.x, 0.1f);
			ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.1f);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Reticle")) {
			ImGui::DragFloat3("position", &Reticle3Dwt_.translation_.x, 0.1f);
			spritePos = reticle_->GetPosition();
			ImGui::DragFloat2("2dpos", &spritePos.x, 0.1f);
			reticle_->SetPosition(spritePos);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("view")) {
			ImGui::DragFloat3("position", &viewProjection_->translation_.x, 0.1f);
			ImGui::DragFloat3("rotation", &viewProjection_->rotation_.x, 0.1f);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Bullet Status")) {

			// LeftBullet_ が存在する場合、IsFire() の結果を表示
			if (LeftBullet_) {
				bool isFire = LeftBullet_->IsFire();
				ImGui::Text("Left IsFire: %s", isFire ? "true" : "false");
				ImGui::DragFloat3("Left bulletPos", &LeftBulletOffset.x, 0.1f);
				ImGui::SliderAngle("Left rotateX", &LeftRotateOffset.x);
				ImGui::SliderAngle("Left rotateY", &LeftRotateOffset.y);
				ImGui::SliderAngle("Left rotateZ", &LeftRotateOffset.z);
			}
			else {
				ImGui::Text("Left IsFire: false (No bullet)");
			}

			// RightBullet_ が存在する場合、IsFire() の結果を表示
			if (RightBullet_) {
				bool isFire = RightBullet_->IsFire();
				ImGui::Text("Right IsFire: %s", isFire ? "true" : "false");
				ImGui::DragFloat3("Right bulletPos", &RightBulletOffset.x, 0.1f);
				ImGui::SliderAngle("Right rotateX", &RightRotateOffset.x);
				ImGui::SliderAngle("Right rotateY", &RightRotateOffset.y);
				ImGui::SliderAngle("Right rotateZ", &RightRotateOffset.z);
			}
			else {
				ImGui::Text("Right IsFire: false (No bullet)");
			}

			// RightBullet_ が存在する場合、IsFire() の結果を表示
			if (CenterBullet_) {
				bool isFire = CenterBullet_->IsFire();
				ImGui::Text("Center IsFire: %s", isFire ? "true" : "false");
				ImGui::DragFloat3("Center bulletPos", &CenterBulletOffset.x, 0.1f);
				ImGui::SliderAngle("Center rotateX", &CenterRotateOffset.x);
				ImGui::SliderAngle("Center rotateY", &CenterRotateOffset.y);
				ImGui::SliderAngle("Center rotateZ", &CenterRotateOffset.z);
			}
			else {
				ImGui::Text("Center IsFire: false (No bullet)");
			}

			ImGui::EndTabItem();  // "Bullet Status" のタブを閉じる
		}
		ImGui::EndTabBar();  // "player" のタブバーを閉じる
	}
}

void Player::Attack() {
	if (input_->PushMouseButton(0)) {
		LeftBullet_->SetFire(true);
		RightBullet_->SetFire(true);
		CenterBullet_->SetFire(true);
	}
	else {
		LeftBullet_->SetFire(false);
		RightBullet_->SetFire(false);
		CenterBullet_->SetFire(false);
	}
}

Vector3 Player::GetWorldReticlePosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = Reticle3Dwt_.matWorld_.m[3][0];
	worldPos.y = Reticle3Dwt_.matWorld_.m[3][1];
	worldPos.z = Reticle3Dwt_.matWorld_.m[3][2];

	return worldPos;
}

void Player::OnCollision() {}

void Player::TramsformReticle()
{
	Vector3 positionReticle = GetWorldReticlePosition();

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewPortMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);

	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = viewProjection_->matView_ * viewProjection_->matProjection_ * matViewport;

	// ワールド->スクリーン座標変換(ここで3Dから2Dになる)
	positionReticle = Transformation(positionReticle, matViewProjectionViewport);

	// スプライトのレティクル座標に設定
	reticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
}

void Player::MoveAim()
{
	// マウス座標を取得
	POINT mousePosition;
	GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	// 2Dレティクルの位置を設定
	Vector2 mousePos2D(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
	reticle_->SetPosition(mousePos2D);

	// 2Dレティクル位置を使ってレイを生成する
	Vector3 nearPoint = Vector3(mousePos2D.x, mousePos2D.y, 0.0f); // ニアクリップ面
	Vector3 farPoint = Vector3(mousePos2D.x, mousePos2D.y, 1.0f);  // ファークリップ面

	// ビューポート行列逆変換を適用してスクリーン座標からNDCに戻す
	Matrix4x4 invMatViewport = Inverse(MakeViewPortMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1));
	nearPoint = Transformation(nearPoint, invMatViewport);
	farPoint = Transformation(farPoint, invMatViewport);

	// ビュー・プロジェクション行列の逆行列を使用してワールド座標に変換
	Matrix4x4 invViewProjection = Inverse(viewProjection_->matView_ * viewProjection_->matProjection_);
	nearPoint = Transformation(nearPoint, invViewProjection);
	farPoint = Transformation(farPoint, invViewProjection);

	// nearPointとfarPointからレイを計算
	Vector3 rayDirection = (farPoint - nearPoint).Normalize();

	// カメラからの距離を調整して3Dレティクル位置を決定
	float distance = 80.0f; // 適切な距離を設定
	Vector3 reticle3DPos = nearPoint + rayDirection * distance;

	// 3Dレティクルのワールド座標を更新
	Reticle3Dwt_.translation_ = reticle3DPos;
	Reticle3Dwt_.UpdateMatrix();
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
	LeftBullet_->SetParent(parent);
	RightBullet_->SetParent(parent);
	CenterBullet_->SetParent(parent);
}

void Player::AimBulletAtReticle() {
	// 3Dレティクルの位置を取得
	Vector3 reticlePosition = GetWorldReticlePosition();

	// カメラの回転（親の回転）を取得し、ワールド空間における調整を行う
	Vector3 parentRotation = CameraRotate;

	// 左の弾の処理
	Vector3 leftDirection = (reticlePosition - LeftBullet_->GetWorldPosition()).Normalize();
	Vector3 leftRotation = CalculateEulerRotationFromDirection(leftDirection);
	leftRotation -= parentRotation; // カメラの回転を補正として引く
	LeftBullet_->SetRotation(leftRotation);

	// 右の弾の処理
	Vector3 rightDirection = (reticlePosition - RightBullet_->GetWorldPosition()).Normalize();
	Vector3 rightRotation = CalculateEulerRotationFromDirection(rightDirection);
	rightRotation -= parentRotation; // カメラの回転を補正として引く
	RightBullet_->SetRotation(rightRotation);

	// 中央の弾の処理
	Vector3 centerDirection = (reticlePosition - CenterBullet_->GetWorldPosition()).Normalize();
	Vector3 centerRotation = CalculateEulerRotationFromDirection(centerDirection);
	centerRotation -= parentRotation; // カメラの回転を補正として引く
	CenterBullet_->SetRotation(centerRotation);
}

// 方向ベクトルからオイラー角を計算する関数
Vector3 Player::CalculateEulerRotationFromDirection(const Vector3& direction) {
	// Z軸方向への回転角度（yaw）を計算
	float yaw = atan2f(direction.x, direction.z);

	// Y軸方向への回転角度（pitch）を計算
	float pitch = -atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));

	// オイラー角を返す
	return Vector3(pitch, yaw, 0.0f); // rollはゼロ
}
