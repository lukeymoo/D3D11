#include "Camera.h"

Camera::Camera(int ww, int wh)
	: 
	lookAt(0.0f, 0.0f, 0.0f),
	viewWidth(ww),
	viewHeight(wh),
	cBufMatrix(DirectX::XMMatrixIdentity())
{
	// set initial camera position
	position = DirectX::XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);

	// set initial lookAt
	DirectX::XMVECTOR temp = DEFAULT_FORWARD_VECTOR;
	DirectX::XMStoreFloat3(&lookAt, temp);
	
	// set initial rotation vector
	rotVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	// set upVector
	upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	return;
}

Camera::~Camera(void)
{
	return;
}

// Disallow >1.41f in pitch (<1.41max, 0, 0>)
void Camera::rotate(XMVECTOR rotateBy) noexcept
{
	XMFLOAT3 current;
	XMFLOAT3 toapply;
	bool isLookingUp = false;

	XMStoreFloat3(&current, rotVector);
	XMStoreFloat3(&toapply, rotateBy);

	double testedv = current.x;
	testedv += toapply.x;

	// test if pitch will be out of range
	if ( testedv >= 1.41 || testedv <= -1.41) {
		// change pitch to remaining allowable pitch
		isLookingUp = (current.x < 1.0f) ? true : false;
		if (isLookingUp) {
			toapply.x = -(current.x + 1.41f);
		}
		else {
			toapply.x = 1.41f - current.x;
		}
	}

	rotateBy = XMVectorSet(toapply.x, toapply.y, toapply.z, 0.0f);
	// apply camera rotation
	rotVector += rotateBy;
	return;
}


// ensures cBufMatrix represents a matrix using most recent
// variables/position coordinates
void Camera::update(void) noexcept
{
	XMVECTOR vLookAt;
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX projectionMatrix = XMMatrixIdentity();
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);


	vLookAt = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, rotMatrix);

	// offset target by camera position
	vLookAt += position;

	// calculate up direction based on camera rotation
	XMVECTOR vUp = XMVector3TransformCoord(DEFAULT_UP_VECTOR, rotMatrix);

	// Create a view matrix using camera position data
	viewMatrix = DirectX::XMMatrixLookAtLH(position, vLookAt, upVector);

	// create projection matrix using FOV data
	projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);

	// Create output matrix -- store in cBufMatrix
	cBufMatrix = worldMatrix * viewMatrix * projectionMatrix;
	return;
}

/*
	GETTERS
*/
DirectX::XMFLOAT3 Camera::getRotationFloat3(void) noexcept
{
	DirectX::XMFLOAT3 t;
	DirectX::XMStoreFloat3(&t, rotVector);
	return t;
}

float Camera::getX(void) const noexcept
{
	DirectX::XMFLOAT3 t;
	DirectX::XMStoreFloat3(&t, position);
	return t.x;
}

float Camera::getY(void) const noexcept
{
	DirectX::XMFLOAT3 t;
	DirectX::XMStoreFloat3(&t, position);
	return t.y;
}

float Camera::getZ(void) const noexcept
{
	DirectX::XMFLOAT3 t;
	DirectX::XMStoreFloat3(&t, position);
	return t.z;
}

DirectX::XMMATRIX Camera::getTransposed(void) noexcept
{
	return DirectX::XMMatrixTranspose(cBufMatrix);
}

/*
	CAMERA MOVEMENT
*/
// X AXIS
void Camera::moveLeft(double fpsdt) noexcept
{
	// get rotation matrix based on camera
	DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(rotVector);
	DirectX::XMVECTOR left_vector = DirectX::XMVector3Transform(DEFAULT_LEFT_VECTOR, rotMatrix);
	left_vector *= moveSpeed * static_cast<float>(fpsdt);
	position += left_vector;
	return;
}

void Camera::moveRight(double fpsdt) noexcept
{
	DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(rotVector);
	DirectX::XMVECTOR right_vector = DirectX::XMVector3Transform(DEFAULT_RIGHT_VECTOR, rotMatrix);
	right_vector *= moveSpeed * static_cast<float>(fpsdt);;
	position += right_vector;
	return;
}

// Y AXIS
void Camera::moveUp(double fpsdt) noexcept
{
	DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(rotVector);
	DirectX::XMVECTOR up_vector = DirectX::XMVector3Transform(DEFAULT_UP_VECTOR, rotMatrix);
	up_vector *= moveSpeed * static_cast<float>(fpsdt);;
	position += up_vector;
	return;
}

void Camera::moveDown(double fpsdt) noexcept
{
	DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(rotVector);
	DirectX::XMVECTOR down_vector = DirectX::XMVector3Transform(DEFAULT_DOWN_VECTOR, rotMatrix);
	down_vector *= moveSpeed * static_cast<float>(fpsdt);;
	position += down_vector;
	return;
}

// Z AXIS
void Camera::moveForward(double fpsdt) noexcept
{
	DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(rotVector);
	DirectX::XMVECTOR forward_vector = DirectX::XMVector3Transform(DEFAULT_FORWARD_VECTOR, rotMatrix);
	forward_vector *= moveSpeed * static_cast<float>(fpsdt);;
	position += forward_vector;
	return;
}


void Camera::moveBackward(double fpsdt) noexcept
{
	DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(rotVector);
	DirectX::XMVECTOR backward_vector = DirectX::XMVector3Transform(DEFAULT_BACKWARD_VECTOR, rotMatrix);
	backward_vector *= moveSpeed * static_cast<float>(fpsdt);;
	position += backward_vector;
	return;
}