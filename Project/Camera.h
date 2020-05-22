#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
	public:
		Camera(int windowWidth, int windowHeight);
		~Camera(void);

		/*
			Camera Position control
		*/
		// X AXIS
		void moveLeft(double fpsdt) noexcept;
		void moveRight(double fpsdt) noexcept;
		// Y AXIS
		void moveUp(double fpsdt) noexcept;
		void moveDown(double fpsdt) noexcept;
		// Z AXIS
		void moveForward(double fpsdt) noexcept;
		void moveBackward(double fpsdt) noexcept;

		void rotate(XMVECTOR rotationVector) noexcept;

		// generates a new matrix with all variables
		// and translations applied
		// this ensures if a position/variable was updated - the matrix represents this change
		void update(void) noexcept;

		/*
			GETTERS
		*/
		XMFLOAT3 getRotationFloat3(void) noexcept;
		float getX(void) const noexcept;
		float getY(void) const noexcept;
		float getZ(void) const noexcept;
		XMMATRIX getTransposed(void) noexcept;

	private:
		int viewWidth;
		int viewHeight;
		XMVECTOR position;
		XMFLOAT3 lookAt;
		XMVECTOR upVector;
		XMVECTOR rotVector;
		const float moveSpeed = 0.005f;

		float fovDegrees = 90.0f; // field of view
		float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
		float aspectRatio = static_cast<float>(viewWidth) / static_cast<float>(viewHeight);
		float nearZ = 0.01f;
		float farZ = 1000.0f;

		const XMVECTOR DEFAULT_UP_VECTOR		= XMVectorSet( 0.0f,  1.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_DOWN_VECTOR		= XMVectorSet( 0.0f, -1.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_FORWARD_VECTOR	= XMVectorSet( 0.0f,  0.0f, 1.0f, 0.0f);
		const XMVECTOR DEFAULT_BACKWARD_VECTOR	= XMVectorSet( 0.0f,  0.0f,-1.0f, 0.0f);
		const XMVECTOR DEFAULT_RIGHT_VECTOR		= XMVectorSet( 1.0f,  0.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_LEFT_VECTOR		= XMVectorSet(-1.0f,  0.0f, 0.0f, 0.0f);

		
		XMMATRIX cBufMatrix;
};