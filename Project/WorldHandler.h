#pragma once

#include <iostream>
#include <fstream>
#include <math.h>

#include <DirectXMath.h>
#include <vector>
#include <wrl/client.h>
#include <d3d11.h>

#include "ExceptionHandler.h"
#include "VertexBuffer.h"

#define WRL Microsoft::WRL

using namespace DirectX;

class WorldHandler
{
	class Exception : public ExceptionHandler
	{
		public:
			Exception(int line, const char* file, HRESULT hr);
			Exception(std::string msg, int line, const char* file);
			~Exception(void);
	};
	public:
		WorldHandler(WRL::ComPtr<ID3D11Device> dV, WRL::ComPtr<ID3D11DeviceContext> dC);
		~WorldHandler(void);

		// ensure 16 byte alignment due to using XMMATRIX class objects
		void* operator new(size_t i)
		{
			return _mm_malloc(i, 16);
		}
		void operator delete(void* p)
		{
			_mm_free(p);
		}

		DWORD getWidth(void) const noexcept;
		DWORD getHeight(void) const noexcept;

		void prepare(void);
		// fetches bitmap data -> converts into non normal positional data
		bool load(std::string filename);
		void draw(XMMATRIX viewMatrix, XMMATRIX projectionMatrix); // draws out the vertices
	private:
		void normalize(void); // normalizes positional data from bitmap
		void store(void); // generate line list from normalized positional data

		std::unique_ptr<VertexBuffer>			vertexBuffer;
		std::unique_ptr<std::vector<Vertex>>	pVertices;

		// local storage of latest transformation matrix
		XMMATRIX cbMatrix;
		WRL::ComPtr<ID3D11Buffer>				pWorldMatrix; // ptr to pipeline storage

		tdPoint* terrainData; // converted bitmap data -> positional belt
		int terrainWidth;
		int terrainHeight;
		bool heightmap_has_been_loaded = false;

		WRL::ComPtr<ID3D11Device>				pDevice;
		WRL::ComPtr<ID3D11DeviceContext>		pDeviceContext;
};

#define WORLDH_EXCEPT(hr) throw WorldHandler::Exception::Exception(__LINE__, __FILE__, hr)
#define WORLD_EXCEPT(str) throw WorldHandler::Exception::Exception(str, __LINE__, __FILE__)