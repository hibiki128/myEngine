#include "Camera.h"
#include "math/myMath.h"
#include "WinApp.h"

Camera::Camera()
	:transform({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} })
	, fovY(0.45f)
	, aspectRatio(float(WinApp::kClientWidth)/float(WinApp::kClientHeight))
	, nearClip(0.1f)
	, farClip(100.0f)
	, worldMatrix(MakeAffineMatrix(transform.scale,transform.rotate,transform.translate))
	, viewMatrix(Inverse(worldMatrix))
	, projectionMatrix(MakePerspectiveFovMatrix(fovY,aspectRatio,nearClip,farClip))
	, viewProjectionMatrix(viewMatrix *projectionMatrix)
{}

void Camera::Update()
{
	worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	viewMatrix = Inverse(worldMatrix);
	projectionMatrix = MakePerspectiveFovMatrix(fovY, aspectRatio, nearClip, farClip);
<<<<<<< HEAD
	viewProjectionMatrix = viewMatrix * projectionMatrix;
=======
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
>>>>>>> 588d19cc7ec69ea69ec2b4e7b1eb2ed8ab6bceb3
}