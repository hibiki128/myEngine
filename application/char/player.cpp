#include "Player.h"
#include <algorithm>
#include <cassert>
#include <imgui.h>

Player::~Player() {
	// bulletの解放
	for (playerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Initilaize(ViewProjection* viewProjection, const Vector3& position) {
	// 引数として受け取ったデータをメンバ変数に記録する
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("suzannu.obj");

	reticle_ = std::make_unique<Sprite>();
	reticle_->Initialize("Reticle.png", { 640.0f,360.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f });

	reticleObj_ = std::make_unique<Object3d>();
	reticleObj_->Initialize("ICO.obj");

	spritePos = reticle_->GetPosition();

	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = { 0.3f,0.3f,0.3f };
	
	Reticle3Dwt_.Initialize();
	Reticle3Dwt_.scale_ = { 3.0f,3.0f,3.0f };

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void Player::Update() {

	// 弾の削除
	Bulletdelete();

	// 旋回処理
	//Rotate();

	// 移動処理
	//Move();

	// 攻撃処理
	Attack();

	SetReticle();

	TramsformReticle();

	MoveAim();

	// 弾更新
	for (playerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 行列更新
	worldTransform_.UpdateMatrix();
}

void Player::Draw() {
	obj_->Draw(worldTransform_, *viewProjection_);
	reticleObj_->Draw(Reticle3Dwt_, *viewProjection_);
	// 弾描画
	for (playerBullet* bullet : bullets_) {
		bullet->Draw(*viewProjection_);
	}
}

void Player::DrawUI()
{
	reticle_->Draw();
}

void Player::imgui()
{
	// キャラクターの座標を画面表示する処理
	if (ImGui::BeginTabBar("player")) {
		if (ImGui::BeginTabItem("Player")) {
			ImGui::DragFloat3("position", &worldTransform_.translation_.x, 0.1f);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Reticle")) {
			ImGui::DragFloat3("position", &Reticle3Dwt_.translation_.x, 0.1f);
			spritePos = reticle_->GetPosition();
			ImGui::DragFloat2("2dpos", &spritePos.x, 0.1f);
			reticle_->SetPosition(spritePos);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void Player::Rotate() {
	// 回転の速さ[ラジアン/flame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}
	else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}
}

void Player::Move() {
	// キャラクターの移動ベクトル
	Vector3 move = { 0, 0, 0 };

	// 　キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	}
	else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	}
	else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	// 移動ベクトルの加算
	worldTransform_.translation_ += move;

	// 移動限界座標
	const float kMoveLimitX = 35.0f;
	const float kMoveLimitY = 19.5f;

	// 範囲を超えない処理
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -kMoveLimitX, +kMoveLimitX);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, -kMoveLimitY, +kMoveLimitY);
}

void Player::Attack() {
	if (input_->TriggerMouseButton(0)) {

		// 弾の速度
		const float kBulletSpeed = 0.5f;
		Vector3 velocity(0, 0, kBulletSpeed);

		// 自機から照準オブジェクトへのベクトル
		velocity = GetWorldReticlePosition() - GetWorldPosition();
		velocity = velocity.Normalize() * kBulletSpeed;

		// 弾を生成し、初期化
		playerBullet* newBullet = new playerBullet();
		newBullet->Initialize(GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

void Player::Bulletdelete() {
	// デスフラグの立った弾を削除
	bullets_.remove_if([](playerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});
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

void Player::SetReticle()
{
	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;
	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset = { 0, 0, 1.0f };
	// 自機のワールド行列の回転を反映
	offset = TransformNormal(offset, worldTransform_.matWorld_);
	// ベクトルの長さを整える
	offset = offset.Normalize() * kDistancePlayerTo3DReticle;
	// 3Dレティクルの座標を設定
	Reticle3Dwt_.translation_ = GetWorldPosition() + offset;
	Reticle3Dwt_.UpdateMatrix();
}

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
	// マウスでの操作
	POINT mousePosition;
	// マウス座標（スクリーン座標）を取得する
	GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換する
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	// スプライトのレティクル座標に設定
	reticle_->SetPosition(Vector2(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)));

	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matViewport = MakeViewPortMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
	Matrix4x4 matVPV = viewProjection_->matView_ * viewProjection_->matProjection_ * matViewport;
	Matrix4x4 matInverseVPV = Inverse(matVPV);

	// スクリーン座標
	Vector3 posNear = Vector3(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 0);
	Vector3 posFar = Vector3(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 1);

	// スクリーン座標系からワールド座標系へ
	posNear = Transformation(posNear, matInverseVPV);
	posFar = Transformation(posFar, matInverseVPV);

	// マウスレイの方向
	Vector3 mouseDirection = posFar - posNear; // posNearからposFarへのベクトルを計算
	mouseDirection = mouseDirection.Normalize();
	// カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 100.0f;
	Reticle3Dwt_.translation_ = posNear + mouseDirection * kDistanceTestObject; // posNearからmouseDirectionの方向にkDistanceTestObject進んだ距離
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
}
