#pragma once

#include <DirectXMath.h>

#include "Mesh.h"

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

		// temp
		void moveForward(void) noexcept;
		void moveBackward(void) noexcept;
		void moveLeft(void) noexcept;
		void moveRight(void) noexcept;

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
		int startIndex = 0;
		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


		WRL::ComPtr<ID3D11Device>			device;
		WRL::ComPtr<ID3D11DeviceContext>	deviceContext;
		WRL::ComPtr<ID3D11Buffer>			pWorldMatrix;

		std::vector<Mesh> meshes;

		std::string model_path;
		bool model_has_been_initialised = false;

		bool loadModel(void);
};