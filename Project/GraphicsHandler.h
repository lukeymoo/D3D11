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

		void createDeviceAndSwapChain(void);
		void initDebugLayer(void);
		void createRenderTarget(void);
		void createVertexBuffer(void);
		void createDepthBuffer(void);
		void configureInputLayout(void);
		void initShaders(void);
		void configureRasterizer(void);
		void initMatrixBuffer(void);
		void configureViewport(void);

		void drawGrid(void);
		void prepareScene(void);

		Model model_1;

		Camera camera;

	private:
		static constexpr float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		HWND hWnd;
		int width;
		int height;

		D3D_FEATURE_LEVEL supportedFeatureLevel;

		/*
			D3D11 Interfaces
		*/
		WRL::ComPtr<IDXGISwapChain>				pSwap; // encapsulates all buffers
		WRL::ComPtr<ID3D11Device>				pDevice; // resource handler
		WRL::ComPtr<ID3D11DeviceContext>		pDeviceContext; // rendering handler
		WRL::ComPtr<ID3D11RasterizerState>		pRasterizerState; // raster stage control
		WRL::ComPtr<ID3D11DepthStencilState>	pDepthStencilState; // depth testing control

		/*
			Resources
		*/
		D3D11_VIEWPORT Viewport;
		WRL::ComPtr<ID3D11RenderTargetView>		pBuffer; // this is our back buffer
		WRL::ComPtr<ID3D11Buffer>				pConstantBuffer; // used for vertex transformations
		WRL::ComPtr<ID3D11Texture2D>			pDepthBuffer; // our depth buffer

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