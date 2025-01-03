#include "Quaternion.h"
#include"numbers"

void Quaternion::SetFromTo(const Vector3& from, const Vector3& to)
{
	Vector3 f = from.Normalize(); // 正規化したfromベクトル
	Vector3 t = to.Normalize();   // 正規化したtoベクトル

	Vector3 cross = f.Cross(t);    // fromとtoのクロス積
	float dot = f.Dot(t);          // fromとtoの内積

	// 回転角をクォータニオンに変換
	w = sqrt((1.0f + dot) * 0.5f); // 回転のスカラー成分
	float s = 0.5f / w;

	x = cross.x * s;
	y = cross.y * s;
	z = cross.z * s;
}

Quaternion Quaternion::FromEuler(const Vector3& euler)
{
	float cy = std::cos(euler.z * 0.5f);
	float sy = std::sin(euler.z * 0.5f);
	float cp = std::cos(euler.y * 0.5f);
	float sp = std::sin(euler.y * 0.5f);
	float cr = std::cos(euler.x * 0.5f);
	float sr = std::sin(euler.x * 0.5f);

	Quaternion q;
	q.w = cr * cp * cy + sr * sp * sy;
	q.x = sr * cp * cy - cr * sp * sy;
	q.y = cr * sp * cy + sr * cp * sy;
	q.z = cr * cp * sy - sr * sp * cy;

	return q;
}

Quaternion Quaternion::FromMatrix(const Matrix4x4& mat)
{
	Quaternion q;
	// 回転行列からクォータニオンを生成するロジック（一般的なアルゴリズムを使用）
	float trace = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];
	if (trace > 0.0f) {
		float s = 0.5f / std::sqrt(trace + 1.0f);
		q.w = 0.25f / s;
		q.x = (mat.m[2][1] - mat.m[1][2]) * s;
		q.y = (mat.m[0][2] - mat.m[2][0]) * s;
		q.z = (mat.m[1][0] - mat.m[0][1]) * s;
	}
	else {
		if (mat.m[0][0] > mat.m[1][1] && mat.m[0][0] > mat.m[2][2]) {
			float s = 2.0f * std::sqrt(1.0f + mat.m[0][0] - mat.m[1][1] - mat.m[2][2]);
			q.w = (mat.m[2][1] - mat.m[1][2]) / s;
			q.x = 0.25f * s;
			q.y = (mat.m[0][1] + mat.m[1][0]) / s;
			q.z = (mat.m[0][2] + mat.m[2][0]) / s;
		}
		else if (mat.m[1][1] > mat.m[2][2]) {
			float s = 2.0f * std::sqrt(1.0f + mat.m[1][1] - mat.m[0][0] - mat.m[2][2]);
			q.w = (mat.m[0][2] - mat.m[2][0]) / s;
			q.x = (mat.m[0][1] + mat.m[1][0]) / s;
			q.y = 0.25f * s;
			q.z = (mat.m[1][2] + mat.m[2][1]) / s;
		}
		else {
			float s = 2.0f * std::sqrt(1.0f + mat.m[2][2] - mat.m[0][0] - mat.m[1][1]);
			q.w = (mat.m[1][0] - mat.m[0][1]) / s;
			q.x = (mat.m[0][2] + mat.m[2][0]) / s;
			q.y = (mat.m[1][2] + mat.m[2][1]) / s;
			q.z = 0.25f * s;
		}
	}
	return q;
}

Vector3 Quaternion::ToEuler() const
{
	Vector3 euler;

	// クォータニオンを正規化
	Quaternion q = Normalize();

	// オイラー角を計算
	euler.x = std::atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)); // Roll
	euler.y = std::asin(2.0f * (q.w * q.y - q.z * q.x));                                         // Pitch
	euler.z = std::atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)); // Yaw

	return euler;
}

Quaternion Quaternion::Conjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::Normalize() const
{
	float length = sqrtf(x * x + y * y + z * z + w * w);
	if (length > 0.0f) {
		return Quaternion(x / length, y / length, z / length, w / length);
	}
	return Quaternion(0, 0, 0, 1); // 単位クォータニオンを返す
}

Quaternion Quaternion::FromLookRotation(const Vector3& direction, const Vector3& up)
{
	Vector3 forward = direction.Normalize();
	Vector3 right = up.Cross(forward).Normalize();
	Vector3 newUp = forward.Cross(right);

	// 回転行列の要素からクォータニオンを計算
	float w = sqrtf(1.0f + right.x + newUp.y + forward.z) * 0.5f;
	float x = (newUp.z - forward.y) / (4.0f * w);
	float y = (forward.x - right.z) / (4.0f * w);
	float z = (right.y - newUp.x) / (4.0f * w);

	return Quaternion(w, x, y, z).Normalize();
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
	return Quaternion(
		w * q.w - x * q.x - y * q.y - z * q.z,                      // スカラー成分
		w * q.x + x * q.w + y * q.z - z * q.y,                      // x成分
		w * q.y - x * q.z + y * q.w + z * q.x,                      // y成分
		w * q.z + x * q.y - y * q.x + z * q.w                       // z成分
	);
}

Quaternion Quaternion::operator+(const Quaternion& other) const
{
	return { x + other.x, y + other.y, z + other.z, w + other.w };
}

Quaternion Quaternion::operator-(const Quaternion& other) const
{
	return { x - other.x, y - other.y, z - other.z, w - other.w };
}

Quaternion Quaternion::operator/(const Quaternion& other) const
{
	Quaternion inverse = other.Inverse();
	return *this * inverse;
}

Quaternion Quaternion::operator*(const float& scalar) const
{
	return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar);
}

Quaternion Quaternion::IdentityQuaternion()
{
	return { 0.0f, 0.0f, 0.0f, 1.0f };
}

float Quaternion::Norm() const
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

float Quaternion::Dot(const Quaternion& other) const
{
	return x * other.x + y * other.y + z * other.z + w * other.w;
}


Quaternion Quaternion::Inverse() const
{
	float normSquared = Norm();
	normSquared *= normSquared;
	if (normSquared == 0.0f) {
		return IdentityQuaternion();
	}
	Quaternion conjugate = Conjugate();
	return { conjugate.x / normSquared, conjugate.y / normSquared, conjugate.z / normSquared, conjugate.w / normSquared };
}

Quaternion Quaternion::Sleap(Quaternion q1, Quaternion q2, float t)
{
	// クォータニオンの内積を計算
	float dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

	// ドット積が負の場合、逆の方向に補間するために q2 を反転
	if (dot < 0.0f) {
		q2.x = -q2.x;
		q2.y = -q2.y;
		q2.z = -q2.z;
		q2.w = -q2.w;
		dot = -dot;
	}

	// 補間係数を使った係数の計算
	const float threshold = 0.9995f;
	if (dot > threshold) {
		// ドット積が閾値を超えた場合、線形補間を実行（角度が小さいため）
		Quaternion result = {
			q1.x + t * (q2.x - q1.x),
			q1.y + t * (q2.y - q1.y),
			q1.z + t * (q2.z - q1.z),
			q1.w + t * (q2.w - q1.w)
		};
		return result.Normalize(); // 結果を正規化
	}

	// 角度の計算
	float theta_0 = std::acos(dot);        // θ0 = q1 と q2 間の角度
	float theta = theta_0 * t;             // θ = t に対応する角度

	// 係数の計算
	float sin_theta = std::sin(theta);
	float sin_theta_0 = std::sin(theta_0);

	float s1 = std::cos(theta) - dot * sin_theta / sin_theta_0;
	float s2 = sin_theta / sin_theta_0;

	// 補間結果の計算
	Quaternion result = {
		s1 * q1.x + s2 * q2.x,
		s1 * q1.y + s2 * q2.y,
		s1 * q1.z + s2 * q2.z,
		s1 * q1.w + s2 * q2.w
	};
	return result;
}
