#include "GraphicsHandler.h"

//std::vector<Vertex> vertices = {
//	{ { -0.5f, 0.0f, 0.100f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // bottom left front tri	-- 1
//	{ {  0.0f, 0.7f, 0.100f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // top of front tri		-- 2
//	{ {  0.5f, 0.0f, 0.100f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // bottom right front tri	-- 3
//	{ { -0.5f, 0.0f, 0.500f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // bottom left back tri	-- 4
//	{ {  0.0f, 0.7f, 0.500f }, { 1.0f, 1.0f, 0.0f, 1.0f } }, // top of back tri			-- 5
//	{ {  0.5f, 0.0f, 0.500f }, { 0.0f, 1.0f, 1.0f, 1.0f } }, // bottom right back tri	-- 6
//};
//
//std::vector<unsigned short> indices = { 1, 2, 3, 1, 4, 2, 2, 5, 4, 3, 6, 2, 5, 2, 6, 4, 5, 6 };

D3D_FEATURE_LEVEL d3dFeatureLevels[] =
{
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1
};

// Here we configure DirectX pipeline
Graphics::Graphics(HWND hw, int w, int h)
	:
	hWnd(hw), width(w), height(h), supportedFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	camera(w, h)
{
	createDeviceAndSwapChain();
	initDebugLayer();
	createRenderTarget();
	createVertexBuffer();
	createDepthBuffer();
	configureInputLayout();
	initShaders();
	configureRasterizer();
	initMatrixBuffer();
	configureViewport();
	model_1.init(pDevice, pDeviceContext, "data\\nanosuit\\nanosuit.obj");
	return;
}

Graphics::~Graphics(void)
{
	return;
}

void Graphics::createDeviceAndSwapChain(void)
{
	HRESULT hr;

	// Swap chain descriptor
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60; // 60 hz
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2; // non full screen buffer
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Create D3D device, context & swap chain
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		d3dFeatureLevels,
		ARRAYSIZE(d3dFeatureLevels),
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		&supportedFeatureLevel,
		&pDeviceContext
	);

	if (FAILED(hr)) {
		GH_EXCEPT(hr);
	}
	return;
}

void Graphics::initDebugLayer(void)
{
	HRESULT hr;
	// get handle to debug layer interface
	hr = pDevice->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(pDebug.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) {
		GH_EXCEPT(hr);
	}

	// configure debugging to be more strict
	hr = pDebug->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);
	if (FAILED(hr)) {
		GH_EXCEPT(hr);
	}
	hr = pDebug->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
	if (FAILED(hr)) {
		GH_EXCEPT(hr);
	}
	hr = pDebug->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
	if (FAILED(hr)) {
		GH_EXCEPT(hr);
	}
	hr = pDebug->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_MESSAGE, true);
	if (FAILED(hr)) {
		GH_EXCEPT(hr);
	}
	return;
}

void Graphics::createRenderTarget(void)
{
	HRESULT hr;
	// create render target to back buffer
	WRL::ComPtr<ID3D11Texture2D> temp;
	hr = pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(temp.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) {
		GH_EXCEPT(hr);
	}

	// create render target view from buffer handle
	hr = pDevice->CreateRenderTargetView(
		temp.Get(),
		nullptr,
		pBuffer.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr)) {
		GH_EXCEPT(hr);
	}
	return;
}

void Graphics::createVertexBuffer(void)
{
	//HRESULT hr;
	//// Create vertex buffer
	//D3D11_BUFFER_DESC vb;
	//D3D11_SUBRESOURCE_DATA vdata;
	//ZeroMemory(&vb, sizeof(D3D11_BUFFER_DESC));
	//ZeroMemory(&vdata, sizeof(D3D11_SUBRESOURCE_DATA));

	//vb.Usage = D3D11_USAGE_DEFAULT;
	//vb.ByteWidth = sizeof(Vertex) * std::size(points);
	//vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vb.CPUAccessFlags = 0;
	//vb.MiscFlags = 0;
	//vb.StructureByteStride = sizeof(Vertex);

	//vertexByteOffset[0] = 0;
	//vertexByteStride[0] = sizeof(Vertex);

	//vdata.pSysMem = points;
	//vdata.SysMemPitch = 0;
	//vdata.SysMemSlicePitch = 0;

	//// load vertex buffer
	//hr = pDevice->CreateBuffer(&vb, &vdata, pVertexBuffer.ReleaseAndGetAddressOf());
	//if (FAILED(hr)) {
	//	GH_EXCEPT(hr);
	//}
	return;
}

void Graphics::createDepthBuffer(void)
{
	HRESULT hr;
	D3D11_DEPTH_STENCIL_DESC dd;
	D3D11_TEXTURE2D_DESC db;
	D3D11_DEPTH_STENCIL_VIEW_DESC dv;

	ZeroMemory(&dd, sizeof(D3D11_DEPTH_STENCIL_DESC));
	ZeroMemory(&db, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&dv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// create depth buffer
	db.Width = width;
	db.Height = height;
	db.MipLevels = 1;
	db.ArraySize = 1;
	db.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	db.SampleDesc.Count = 1;
	db.SampleDesc.Quality = 0;
	db.Usage = D3D11_USAGE_DEFAULT;
	db.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	db.CPUAccessFlags = 0;
	db.MiscFlags = 0;

	hr = pDevice->CreateTexture2D(&db, nullptr, pDepthBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// create stencil state
	dd.DepthEnable = true;
	dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dd.DepthFunc = D3D11_COMPARISON_LESS;
	dd.StencilEnable = true;
	dd.StencilReadMask = 0xff;
	dd.StencilWriteMask = 0xff;
		// front facing pixels
		dd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		// back facing pixels
		dd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = pDevice->CreateDepthStencilState(&dd, pDepthStencilState.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// create depth view
	dv.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dv.Texture2D.MipSlice = 0;

	hr = pDevice->CreateDepthStencilView(pDepthBuffer.Get(), &dv, pDepthStencilView.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// bind state -> pipeline
	pDeviceContext->OMSetDepthStencilState(pDepthStencilState.Get(), 1);
	return;
}

void Graphics::configureInputLayout(void)
{
	return;
}

void Graphics::initShaders(void)
{
	HRESULT hr;
	WRL::ComPtr<ID3DBlob> vBlob; // vertex shader
	WRL::ComPtr<ID3DBlob> pBlob; // pixel shader
	
	// load shader files into blobs
	hr = D3DReadFileToBlob(L"VertexShader.cso", vBlob.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }
	hr = D3DReadFileToBlob(L"PixelShader.cso", pBlob.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// create vertex and pixel shader from blobs
	hr = pDevice->CreateVertexShader(vBlob->GetBufferPointer(), vBlob->GetBufferSize(), nullptr, pVertexShader.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }
	hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pPixelShader.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// bind shaders
	pDeviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0);
	pDeviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0);
	return;
}

void Graphics::configureRasterizer(void)
{
	HRESULT hr;
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));

	//rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthClipEnable = true;

	hr = pDevice->CreateRasterizerState(&rd, pRasterizerState.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	pDeviceContext->RSSetState(pRasterizerState.Get()); // bind rasterizer -> pipeline
	return;
}


void Graphics::initMatrixBuffer(void)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE s;
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&s, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ZeroMemory(&cbd, sizeof(D3D11_BUFFER_DESC));

	cbd = {
		sizeof(DirectX::XMMATRIX),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0,
		sizeof(float)
	};

	hr = pDevice->CreateBuffer(&cbd, nullptr, pConstantBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// copy transform matrix to new buffer
	hr = pDeviceContext->Map(pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &s);
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	CopyMemory(s.pData, &camera.getTransposed(), sizeof(DirectX::XMMATRIX));

	pDeviceContext->Unmap(pConstantBuffer.Get(), 0);

	pDeviceContext->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf()); // bind buffer -> pipeline
	return;
}

void Graphics::configureViewport(void)
{
	// configure viewport struct
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = static_cast<float>(width);
	Viewport.Height = static_cast<float>(height);
	Viewport.MinDepth = 0;
	Viewport.MaxDepth = 1;
	return;
}

void Graphics::drawGrid(void)
{
	return;
}

void Graphics::prepareScene(void)
{
	HRESULT hr;

	pDeviceContext->ClearRenderTargetView(pBuffer.Get(), clearColor);
	pDeviceContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);
	/*pDeviceContext->IASetVertexBuffers(
		0,
		1,
		pVertexBuffer.GetAddressOf(),
		vertexByteStride,
		vertexByteOffset
	);*/
	//pDeviceContext->IASetInputLayout(pInputLayout.Get());
	//pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->OMSetRenderTargets(1, pBuffer.GetAddressOf(), pDepthStencilView.Get());
	pDeviceContext->RSSetState(pRasterizerState.Get());
	pDeviceContext->RSSetViewports(1, &Viewport);

	// call every model's draw function
	// note -- model's do not apply any view or projection matrices, only world matrix
	model_1.draw();

	// apply camera view matrix to vertex buffer
	D3D11_MAPPED_SUBRESOURCE s;
	ZeroMemory(&s, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = pDeviceContext->Map(pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &s);
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	CopyMemory(s.pData, &camera.getTransposed(), sizeof(DirectX::XMMATRIX));

	pDeviceContext->Unmap(pConstantBuffer.Get(), 0);

	return;
}



/*
	Internal Exception Class
*/
Graphics::Exception::Exception(int l, std::string f, HRESULT h)
	: ExceptionHandler(l, f, h)
{
	type = "Graphics Handler Exception";
	return;
}

Graphics::Exception::Exception(std::string m, int l, const char* f)
	: ExceptionHandler(l, f, 0)
{
	type = "Graphics Handler Exception";
	errorDescription = m;
	return;
}