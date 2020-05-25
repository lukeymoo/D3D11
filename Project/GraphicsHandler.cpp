#include "GraphicsHandler.h"

// Here we configure DirectX pipeline
Graphics::Graphics(HWND hw, int w, int h)
	:
	hWnd(hw), width(w), height(h), supportedFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	camera(w, h)
{
	createDeviceAndSwapChain(D3D11_CREATE_DEVICE_DEBUG);
	createRenderTarget();
	createDepthBuffer();
	initShaders();
	initRasterizer();
	initViewport();

	// creates vertices for world grid
	initGrid();

	model_1.init(pDevice, pDeviceContext, "data\\nanosuit\\nanosuit.obj");


	shouldRender = true;
	return;
}

Graphics::~Graphics(void)
{
	return;
}

void Graphics::initGrid(void)
{
	HRESULT hr;
	// temp containers
	std::vector<Vertex> grid;
	D3D11_BUFFER_DESC gridd;
	D3D11_SUBRESOURCE_DATA griddata;
	WRL::ComPtr<ID3D11Texture2D> tempGrid;

	ZeroMemory(&gridd, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&griddata, sizeof(D3D11_SUBRESOURCE_DATA));

	if (pGridBuffer != nullptr) {
		pGridBuffer.Reset();
	}

	// z plane
	for (float i = -100.0f; i < 100.0f; i++) {
		// rows
		grid.push_back(Vertex(-100.0f, 0.0f, i, 1.0f, 1.0f, 1.0f, 1.0f));
		grid.push_back(Vertex(100.0f, 0.0f, i, 1.0f, 1.0f, 1.0f, 1.0f));

		// columns
		grid.push_back(Vertex(i, 0.0f, -100.0f, 0.0f, 1.0f, 0.0f, 1.0f)); // negative z is green
		grid.push_back(Vertex(i, 0.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f)); // positive z is blue
	}

	// y plane
	for (float i = -100.0f; i < 100.0f; i++) {
		// rows
		grid.push_back(Vertex(-100.0f, i, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f));
		grid.push_back(Vertex(100.0f, i, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f));

		// columns
		grid.push_back(Vertex(i, -100.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f));
		grid.push_back(Vertex(i, 100.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f));
	}

	gridVertices = std::size(grid);

	// create buffer to store vertices

	gridd.ByteWidth = sizeof(Vertex) * std::size(grid);
	gridd.Usage = D3D11_USAGE_DYNAMIC;
	gridd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	gridd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gridd.MiscFlags = 0;
	gridd.StructureByteStride = sizeof(Vertex);
	griddata.pSysMem = static_cast<void*>(grid.data());

	hr = pDevice->CreateBuffer(&gridd, &griddata, pGridBuffer.GetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }
	return;
}

void Graphics::createDeviceAndSwapChain(UINT createDeviceFlags)
{
	HRESULT hr;

	if (pDevice != nullptr) {
		pDevice.Reset();
	}
	if (pDeviceContext != nullptr) {
		pDeviceContext.Reset();
	}
	if (pSwap != nullptr) {
		pSwap.Reset();
	}

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
	sd.BufferCount = 2;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Create D3D device, context & swap chain
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
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

#ifdef DEBUG
	initDebugLayer();
#endif

	return;
}

void Graphics::initDebugLayer(void)
{
	HRESULT hr;

	if (pDebug != nullptr) {
		pDebug.Reset();
	}

	// get handle to debug layer interface
	hr = pDevice->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(pDebug.GetAddressOf()));
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// configure debugging to be more strict
	hr = pDebug->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);
	if (FAILED(hr)) { GH_EXCEPT(hr); }
	hr = pDebug->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
	if (FAILED(hr)) { GH_EXCEPT(hr); }
	hr = pDebug->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
	if (FAILED(hr)) { GH_EXCEPT(hr); }
	hr = pDebug->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_MESSAGE, true);
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	return;
}

void Graphics::createRenderTarget(void)
{
	HRESULT hr;

	if (pBuffer != nullptr) {
		pBuffer.Reset();
	}

	// create render target to back buffer
	WRL::ComPtr<ID3D11Texture2D> temp;
	hr = pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(temp.GetAddressOf()));
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// create render target view from buffer handle
	hr = pDevice->CreateRenderTargetView(
		temp.Get(),
		nullptr,
		pBuffer.GetAddressOf()
	);

	if (FAILED(hr)) {
		GH_EXCEPT(hr);
	}
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

	if (pDepthBuffer != nullptr) {
		pDepthBuffer.Reset();
	}
	if (pDepthStencilState != nullptr) {
		pDepthStencilState.Reset();
	}
	if (pDepthStencilView != nullptr) {
		pDepthStencilView.Reset();
	}

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

	hr = pDevice->CreateTexture2D(&db, nullptr, pDepthBuffer.GetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// create stencil state
	dd.DepthEnable = true;
	dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dd.DepthFunc = D3D11_COMPARISON_LESS;
	dd.StencilEnable = true;
	dd.StencilReadMask = 0xff;
	dd.StencilWriteMask = 0xff;
		// front facing pixels -- drawn clockwise
		dd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		// back facing pixels -- drawn counter clockwise
		dd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = pDevice->CreateDepthStencilState(&dd, pDepthStencilState.GetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// create depth view
	dv.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dv.Texture2D.MipSlice = 0;

	hr = pDevice->CreateDepthStencilView(pDepthBuffer.Get(), &dv, pDepthStencilView.GetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// bind stencil state -> pipeline
	pDeviceContext->OMSetDepthStencilState(pDepthStencilState.Get(), 1);
	return;
}

void Graphics::initShaders(void)
{
	HRESULT hr;
	WRL::ComPtr<ID3DBlob> vBlob; // vertex shader
	WRL::ComPtr<ID3DBlob> pBlob; // pixel shader

	if (pVertexShader != nullptr) {
		pVertexShader.Reset();
	}
	if (pPixelShader != nullptr) {
		pPixelShader.Reset();
	}
	
	// load shader files into blobs
	hr = D3DReadFileToBlob(L"VertexShader.cso", vBlob.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }
	hr = D3DReadFileToBlob(L"PixelShader.cso", pBlob.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// create vertex and pixel shader from blobs
	hr = pDevice->CreateVertexShader(vBlob->GetBufferPointer(), vBlob->GetBufferSize(), nullptr, pVertexShader.GetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }
	hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pPixelShader.GetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// bind shaders to pipeline
	pDeviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0);
	pDeviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0);
	return;
}

void Graphics::initRasterizer(void)
{
	HRESULT hr;
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));

	if (pRasterNormalState != nullptr) {
		pRasterNormalState.Reset();
	}
	if (pRasterDebugState != nullptr) {
		pRasterDebugState.Reset();
	}

	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthClipEnable = true;

	hr = pDevice->CreateRasterizerState(&rd, pRasterNormalState.GetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }

	// rasterizer debugging state
	ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthClipEnable = true;

	hr = pDevice->CreateRasterizerState(&rd, pRasterDebugState.GetAddressOf());
	if (FAILED(hr)) { GH_EXCEPT(hr); }


	// bind normal raster -> pipeline
	pDeviceContext->RSSetState(pRasterNormalState.Get());
	return;
}

void Graphics::initViewport(void)
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

void Graphics::toggleDrawGrid(void) noexcept
{
	shouldDrawGrid = !shouldDrawGrid;
	return;
}

void Graphics::toggleDebugRaster(void) noexcept
{
	useDebugRaster = !useDebugRaster;
	return;
}

void Graphics::setDebugRaster(bool on_off) noexcept
{
	useDebugRaster = on_off;
	return;
}

void Graphics::setDrawGrid(bool on_off) noexcept
{
	shouldDrawGrid = on_off;
	return;
}

void Graphics::drawGrid(void)
{
	// draw all the grid vertices as line list
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	pDeviceContext->IASetVertexBuffers(0, 1, pGridBuffer.GetAddressOf(), gridByteStrides, gridOffset);
	pDeviceContext->Draw(gridVertices, 0);
	return;
}

void Graphics::prepareScene(void)
{

	pDeviceContext->ClearRenderTargetView(pBuffer.Get(), clearColor);
	pDeviceContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);

	pDeviceContext->OMSetRenderTargets(1, pBuffer.GetAddressOf(), pDepthStencilView.Get());
	pDeviceContext->RSSetViewports(1, &Viewport);

	// what raster style to use
	if (useDebugRaster) {
		pDeviceContext->RSSetState(pRasterDebugState.Get());
	}
	else {
		pDeviceContext->RSSetState(pRasterNormalState.Get());
	}

	// world grid
	if (shouldDrawGrid) {
		drawGrid();
	}

	/*
		We pass the Camera's view and projection matrix to the models
		Every mesh is drawn with the view & projection matrix applied to it's internally stored world matrix
	*/
	camera.update();
	model_1.draw(camera.getView(), camera.getProjection());

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