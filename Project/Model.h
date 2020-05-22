#pragma once

#include "Mesh.h"

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
		void draw(void);
		void processNode(aiNode *rootNode, const aiScene *pScene);
		Mesh processMesh(aiMesh *e, const aiScene* pScene);
	private:
		WRL::ComPtr<ID3D11Device> device;
		WRL::ComPtr<ID3D11DeviceContext> deviceContext;

		std::vector<Mesh> meshes;
		bool loadModel(void);

		std::string model_path;
		bool model_has_been_initialised = false;
};