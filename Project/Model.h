#pragma once

#include <DirectXMath.h>

#include "Mesh.h"

#include "GlobalTypes.h"

using namespace DirectX;
#define WRL Microsoft::WRL

class Model
{
	public:
		Model(void);
		~Model(void);

		void init(
			WRL::ComPtr<ID3D11Device> device,
			WRL::ComPtr<ID3D11DeviceContext> deviceContext,
			std::string path_to_model_data);

		void processNode(aiNode *rootNode, const aiScene *pScene);
		Mesh processMesh(aiMesh *e, const aiScene* pScene);

		XMVECTOR getEyePositionVector(void) const noexcept;

		void move(DIRECTION direction, double frameDelta) noexcept;

		void rotateLeft(void) noexcept;
		void rotateRight(void) noexcept;

		void setTopology(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept;
		void setStartIndex(int index) noexcept;


		XMMATRIX modelToWorldMatrix;
		XMVECTOR position;

		void draw(XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
		
		// ensures the model's rotation is the same as the players
		// camera shouldn't be able to look the model it's assigned to in the eyes....
		void update(void) noexcept;
	private:
		/*
			Model movement -- called by wrapper `move()`
		*/
		float moveSpeed = 0.005f;

		void moveForward_Default(void) noexcept;
		void moveBackward_Default(void) noexcept;
		void moveLeft_Default(void) noexcept;
		void moveRight_Default(void) noexcept;
		void moveUp_Default(void) noexcept;
		void moveDown_Default(void) noexcept;
	private:
		void setFrameDelta(double fd) noexcept;
	private:
		/*
			Configuration control / Resources
		*/
		int startIndex = 0;
		float frameDelta = 1; // keep move speed consistent across CPU speeds
		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


		WRL::ComPtr<ID3D11Device>			device;
		WRL::ComPtr<ID3D11DeviceContext>	deviceContext;
		WRL::ComPtr<ID3D11Buffer>			pWorldMatrix;
		WRL::ComPtr<ID3D11InputLayout>		m_InputLayout;

		std::vector<Mesh> meshes;

		std::string model_path;
		bool model_has_been_initialised = false;

		bool loadModel(void);

		const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_DOWN_VECTOR = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
};