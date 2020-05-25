#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <d3dcompiler.h>

#include "ExceptionHandler.h"

#define WRL Microsoft::WRL

struct Vertex
{
	struct
	{
		float x;
		float y;
		float z;
	} position;
	struct
	{
		float r;
		float g;
		float b;
		float a;
	} color;
	Vertex() {
		position.x = 0.0f;
		position.y = 0.0f;
		position.z = 0.0f;
		color.r = 1.0f;
		color.g = 1.0f;
		color.b = 1.0f;
		color.a = 1.0f;
	};
	Vertex(float x, float y, float z, float r, float g, float b, float a) {
		position.x = x;
		position.y = y;
		position.z = z;
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}
};

class VertexBuffer
{
	friend class Model;
	class Exception : public ExceptionHandler
	{
	public:
		Exception(int l, std::string f, HRESULT h); // for HRESULTS
		Exception(std::string nohrexception, int li, const char* fi); // non hr
		~Exception(void) = default;
	};
	public:
		VertexBuffer(
			WRL::ComPtr<ID3D11Device> device,
			WRL::ComPtr<ID3D11DeviceContext> deviceContext,
			std::vector<Vertex> vertices,
			std::vector<unsigned short> indices
		);
		~VertexBuffer(void);

		unsigned int getNumIndices(void) const noexcept;
	
	private:
		unsigned int indiceCount = 0;
	public:
		// vertex buffer formatting descriptors
		UINT byteStride[1];
		UINT byteOffset[1];

		WRL::ComPtr<ID3D11Device> device;
		WRL::ComPtr<ID3D11DeviceContext> deviceContext;

		WRL::ComPtr<ID3D11Buffer> m_VertexBuffer; // define vertices
		WRL::ComPtr<ID3D11Buffer> m_IndexBuffer; // define draw order of vertices
		WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
};

#define VH_EXCEPT(hr) throw VertexBuffer::Exception::Exception(__LINE__, __FILE__, hr)
#define V_EXCEPT(str) throw VertexBuffer::Exception::Exception(str, __LINE__, __FILE__)