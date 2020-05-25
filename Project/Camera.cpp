#include "Camera.h"

Camera::Camera(int ww, int wh)
	: 
	lookAt(0.0f, 0.0f, 0.0f),
	viewWidth(ww),
	viewHeight(wh)
{
	// set initial camera position
	position = XMVectorSet(1.0f, 6.0f, -2.0f, 0.0f);

	// set initial lookAt
	XMVECTOR temp = DEFAULT_FORWARD_VECTOR;
	XMStoreFloat3(&lookAt, temp);
	
	// set initial rotation vector
	rotVector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	// set upVector
	upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	return;
}

Camera::~Camera(void)
{
	return;
}

// Disallow >1.3f
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
	if ( testedv >= 1.3f || testedv <= -1.3f) {
		// change pitch to remaining allowable pitch
		isLookingUp = (current.x < 1.0f) ? true : false;
		if (isLookingUp) {
			toapply.x = -(current.x + 1.3f);
		}
		else {
			toapply.x = 1.3f - current.x;
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
	viewMatrix = XMMatrixIdentity();
	projectionMatrix = XMMatrixIdentity();
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);


	vLookAt = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, rotMatrix);

	// offset target by camera position
	vLookAt += position;

	// calculate up direction based on camera rotation
	XMVECTOR vUp = XMVector3TransformCoord(DEFAULT_UP_VECTOR, rotMatrix);

	// store view matrix using camera position data
	viewMatrix = XMMatrixLookAtLH(position, vLookAt, upVector);

	// store projection matrix using FOV data
	projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
	return;
}

/*
	GETTERS
*/
XMFLOAT3 Camera::getRotationFloat3(void) const noexcept
{
	XMFLOAT3 t;
	XMStoreFloat3(&t, rotVector);
	return t;
}

XMVECTOR Camera::getRotationVector(void) const noexcept
{
	return rotVector;
}

float Camera::getX(void) const noexcept
{
	XMFLOAT3 t;
	XMStoreFloat3(&t, position);
	return t.x;
}

float Camera::getY(void) const noexcept
{
	XMFLOAT3 t;
	XMStoreFloat3(&t, position);
	return t.y;
}

float Camera::getZ(void) const noexcept
{
	XMFLOAT3 t;
	XMStoreFloat3(&t, position);
	return t.z;
}

/*
	CAMERA MOVEMENT
*/
// X AXIS
void Camera::moveLeft(double fpsdt) noexcept
{
	// get rotation matrix based on camera
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);
	XMVECTOR left_vector = XMVector3Transform(DEFAULT_LEFT_VECTOR, rotMatrix);
	left_vector *= moveSpeed * static_cast<float>(fpsdt);
	position += left_vector;
	return;
}

void Camera::moveRight(double fpsdt) noexcept
{
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);
	XMVECTOR right_vector = XMVector3Transform(DEFAULT_RIGHT_VECTOR, rotMatrix);
	right_vector *= moveSpeed * static_cast<float>(fpsdt);;
	position += right_vector;
	return;
}

// Y AXIS
void Camera::moveUp(double fpsdt) noexcept
{
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);
	XMVECTOR up_vector = XMVector3Transform(DEFAULT_UP_VECTOR, rotMatrix);
	up_vector *= moveSpeed * static_cast<float>(fpsdt);;
	position += up_vector;
	return;
}

void Camera::moveDown(double fpsdt) noexcept
{
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);
	XMVECTOR down_vector = XMVector3Transform(DEFAULT_DOWN_VECTOR, rotMatrix);
	down_vector *= moveSpeed * static_cast<float>(fpsdt);;
	position += down_vector;
	return;
}

// Z AXIS
void Camera::moveForward(double fpsdt) noexcept
{
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);
	XMVECTOR forward_vector = XMVector3Transform(DEFAULT_FORWARD_VECTOR, rotMatrix);
	forward_vector *= moveSpeed * static_cast<float>(fpsdt);;
	position += forward_vector;
	return;
}


void Camera::moveBackward(double fpsdt) noexcept
{
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);
	XMVECTOR backward_vector = XMVector3Transform(DEFAULT_BACKWARD_VECTOR, rotMatrix);
	backward_vector *= moveSpeed * static_cast<float>(fpsdt);;
	position += backward_vector;
	return;
}

void Camera::placeAtModel(XMVECTOR npos) noexcept
{
	position = npos;
	update();
}

XMMATRIX Camera::getView(void) const noexcept
{
	return viewMatrix;
}

XMMATRIX Camera::getProjection(void) const noexcept
{
	return projectionMatrix;
}

XMVECTOR Camera::getPositionVector(void) const noexcept
{
	return position;
}

void Camera::setRunning(bool isRunning) noexcept
{
	if (isRunning) {
		moveSpeed = speed * runMultiplier;
	}
	else {
		moveSpeed = speed;
	}
	return;
}