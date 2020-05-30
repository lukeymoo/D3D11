#include "Camera.h"

Camera::Camera(int ww, int wh)
	: 
	lookAt(0.0f, 0.0f, 0.0f),
	viewWidth(ww),
	viewHeight(wh),
	projectionMatrix(XMMatrixIdentity()),
	viewMatrix(XMMatrixIdentity())
{
	// set initial camera position
	position = XMVectorSet(0.0f, 8.0f, -8.0f, 0.0f);

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
// wrapper
void Camera::move(DIRECTION dir, double fd) noexcept
{
	// update frame delta for consistent movement across CPU speeds
	setFrameDelta(static_cast<float>(fd));

	// determine camera mode
	switch (cameraMode)
	{
		case CAMERA_MODE::FIRSTPERSON:
		{
			break;
		}
		case CAMERA_MODE::THIRDPERSON:
		{
			break;
		}
		case CAMERA_MODE::FREELOOK:
		{
			// determine direction
			if (dir == DIRECTION::FORWARD) {
				moveForward_Free();
			}
			if (dir == DIRECTION::BACKWARD) {
				moveBackward_Free();
			}
			if (dir == DIRECTION::LEFT) {
				moveLeft_Free();
			}
			if (dir == DIRECTION::RIGHT) {
				moveRight_Free();
			}
			if (dir == DIRECTION::UP) {
				moveUp_Free();
			}
			if (dir == DIRECTION::DOWN) {
				moveDown_Free();
			}
			break;
		}
	}
	return;
}

// X AXIS
void Camera::moveLeft_Free(void) noexcept
{
	// get rotation matrix based on camera
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);
	XMVECTOR left_vector = XMVector3Transform(DEFAULT_LEFT_VECTOR, rotMatrix);
	left_vector *= moveSpeed * frameDelta;
	position += left_vector;
	return;
}

void Camera::moveRight_Free(void) noexcept
{
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);
	XMVECTOR right_vector = XMVector3Transform(DEFAULT_RIGHT_VECTOR, rotMatrix);
	right_vector *= moveSpeed * frameDelta;
	position += right_vector;
	return;
}

// WITH PITCH ADJUSTMENT
// Moves based on new calculated `UP` direction
// IE -> LOOKING STRAIGHT DOWN (90 DEG) PRODUCES AN `UP` DIRECTION ALONG X-Z PLANE
//void Camera::moveUp_Free(void) noexcept
//{
//	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);
//	XMVECTOR up_vector = XMVector3Transform(DEFAULT_UP_VECTOR, rotMatrix);
//	up_vector *= moveSpeed * frameDelta;
//	position += up_vector;
//	return;
//}

// WITH PITCH ADJUSTMENT
// Moves based on new calculated `DOWN` direction
// IE -> LOOKING STRAIGHT DOWN (90 DEG) PRODUCES AN `DOWN` DIRECTION ALONG X-Z PLANE
//void Camera::moveDown_Free(void) noexcept
//{
//	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector);
//	XMVECTOR down_vector = XMVector3Transform(DEFAULT_DOWN_VECTOR, rotMatrix);
//	down_vector *= moveSpeed * frameDelta;
//	position += down_vector;
//	return;
//}

// simply moves up/down Y axis -- no `up` recalculation
void Camera::moveUp_Free(void) noexcept
{
	XMVECTOR up_vector = DEFAULT_UP_VECTOR * moveSpeed * frameDelta;
	position += up_vector;
	return;
}

void Camera::moveDown_Free(void) noexcept
{
	XMVECTOR down_vector = DEFAULT_DOWN_VECTOR * moveSpeed * frameDelta;
	position += down_vector;
	return;
}

// NO PITCH ADJUSTMENT
void Camera::moveForward_Free(void) noexcept
{
	XMMATRIX noPitch = XMMatrixRotationRollPitchYawFromVector(XMVectorSetX(rotVector, 0.0f));
	XMVECTOR forward_vector = XMVector3Transform(DEFAULT_FORWARD_VECTOR, noPitch);
	forward_vector *= moveSpeed * frameDelta;
	position += forward_vector;
	return;
}

// NO PITCH ADJUSTMENT
void Camera::moveBackward_Free(void) noexcept
{
	XMMATRIX noPitch = XMMatrixRotationRollPitchYawFromVector(XMVectorSetX(rotVector, 0.0f));
	XMVECTOR backward_vector = XMVector3Transform(DEFAULT_BACKWARD_VECTOR, noPitch);
	backward_vector *= moveSpeed * frameDelta;
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

CAMERA_MODE Camera::getCameraStyle(void) const noexcept
{
	return cameraMode;
}


void Camera::setCameraStyle(CAMERA_MODE newMode) noexcept
{
	cameraMode = newMode;
	return;
}

void Camera::setFrameDelta(double dt) noexcept
{
	frameDelta = dt;
	return;
}