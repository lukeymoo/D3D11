#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include "winclean.h"
#include <wrl/client.h>
#include <DirectXTex.h>
#include <DirectXMath.h>
#include <string>

#include "VertexBuffer.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

class Mesh
{
	public:
		Mesh(
			WRL::ComPtr<ID3D11Device> device,
			WRL::ComPtr<ID3D11DeviceContext> deviceContext,
			std::vector<Vertex> vertices,
			std::vector<unsigned short> indices
		);
		Mesh(const Mesh& mesh);
		~Mesh(void);
		
	public:
		void draw(void);
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned short> indices;

		VertexBuffer vertexBuffer;
	private:
		WRL::ComPtr<ID3D11Device> device;
		WRL::ComPtr<ID3D11DeviceContext> deviceContext;
};