#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include "winclean.h"
#include <wrl/client.h>
#include <DirectXTex.h>
#include <DirectXMath.h>

#include "ExceptionHandler.h"
#include "Camera.h"
#include "Model.h"
#include "WorldHandler.h"

#define WRL Microsoft::WRL

class Graphics
{
	friend class WindowHandler;
	friend class Model;
	friend class VertexBuffer;
	public:
		class Exception : public ExceptionHandler
		{
			public:
				Exception(int l, std::string f, HRESULT h); // for HRESULTS
				Exception(std::string nohrexception, int li, const char* fi); // non hr
				~Exception(void) = default;
		};
	public:
		Graphics(HWND hWnd, int w, int h);
		~Graphics(void);

		// ensure 16 byte alignment due to using XMMATRIX class objects
		void* operator new(size_t i)
		{
			return _mm_malloc(i, 16);
		}
		void operator delete(void* p)
		{
			_mm_free(p);
		}

		void createDeviceAndSwapChain(UINT createDeviceFlags);
		void createRenderTarget(void);
		void createDepthBuffer(void);
		void initShaders(void);
		void initRasterizer(void);
		void initViewport(void);


		/*
			WORLD GRID
		*/
		void initGrid(void);
		void toggleDrawGrid(void) noexcept;
		void setDrawGrid(bool on_off) noexcept;
		void drawGrid(void);

		void prepareScene(void);
		void toggleDebugRaster(void) noexcept;
		void setDebugRaster(bool on_off) noexcept;

		Model model_1;
		Camera camera;
		WorldHandler worldHandler;
	private:
		static constexpr float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		
		// used internally if DEBUG is defined
		void initDebugLayer(void);

		D3D_FEATURE_LEVEL d3dFeatureLevels[7] =	{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		HWND hWnd;
		int width;
		int height;
		bool fullscreen = false;
		bool shouldRender = false;
		bool shouldDrawGrid = true;
		bool useDebugRaster = false;

		D3D_FEATURE_LEVEL supportedFeatureLevel;

		/*
			D3D11 Interfaces
		*/
		WRL::ComPtr<IDXGISwapChain>				pSwap; // encapsulates all buffers
		WRL::ComPtr<ID3D11Device>				pDevice; // resource handler
		WRL::ComPtr<ID3D11DeviceContext>		pDeviceContext; // rendering handler
		WRL::ComPtr<ID3D11RasterizerState>		pRasterNormalState; // raster fill solid, cull back
		WRL::ComPtr<ID3D11RasterizerState>		pRasterDebugState; // raster wireframe, cull none
		WRL::ComPtr<ID3D11DepthStencilState>	pDepthStencilState; // depth testing control

		/*
			Resources
		*/
		D3D11_VIEWPORT Viewport;
		WRL::ComPtr<ID3D11RenderTargetView>		pBuffer; // this is our back buffer
		WRL::ComPtr<ID3D11Texture2D>			pDepthBuffer; // our depth buffer
		
		WRL::ComPtr<ID3D11Buffer>				pGridBuffer; // grid vertex buffer
		int gridVertices = 0;
		UINT gridByteStrides[1] = { sizeof(Vertex) };
		UINT gridOffset[1] = { 0 };

		WRL::ComPtr<ID3D11VertexShader>			pVertexShader; // our vertex shader
		WRL::ComPtr<ID3D11PixelShader>			pPixelShader; // our pixel shader
		WRL::ComPtr<ID3D11DepthStencilView>		pDepthStencilView; // depth buffer

		WRL::ComPtr<ID3D11InputLayout>			pInputLayout; // tell d3d how to read our vertex buffer

		/*
			Debug
		*/
		WRL::ComPtr<ID3D11InfoQueue>		pDebug; // debug layer interface
};

#define GH_EXCEPT(hr) throw Graphics::Exception::Exception(__LINE__, __FILE__, hr)
#define G_EXCEPT(str) throw Graphics::Exception::Exception(str, __LINE__, __FILE__)