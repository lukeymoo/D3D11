#include "VertexBuffer.h"

/*
	Internal Exception Class
*/
VertexBuffer::Exception::Exception(int l, std::string f, HRESULT h)
	: ExceptionHandler(l, f, h)
{
	type = "Modeler Exception -- Vertex Buffer";
	return;
}

VertexBuffer::Exception::Exception(std::string m, int l, const char* f)
	: ExceptionHandler(l, f, 0)
{
	type = "Modeler Exception -- Vertex Buffer";
	errorDescription = m;
	return;
}

VertexBuffer::VertexBuffer(
	WRL::ComPtr<ID3D11Device> dV,
	WRL::ComPtr<ID3D11DeviceContext> dC,
	std::vector<Vertex> vertices,
	std::vector<unsigned short> indices
)
{
	device = dV;
	deviceContext = dC;

	if (device == nullptr) {
		V_EXCEPT("ID3D11 Device->nullptr");
	}
	HRESULT hr;

	// Create vertex buffer
	D3D11_BUFFER_DESC vb;
	D3D11_SUBRESOURCE_DATA vdata;
	ZeroMemory(&vb, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&vdata, sizeof(D3D11_SUBRESOURCE_DATA));

	vb.Usage = D3D11_USAGE_DEFAULT;
	vb.ByteWidth = sizeof(Vertex) * vertices.size();
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb.CPUAccessFlags = 0;
	vb.MiscFlags = 0;
	vb.StructureByteStride = sizeof(Vertex);
	vdata.pSysMem = static_cast<void*>(vertices.data());
	vdata.SysMemPitch = 0;
	vdata.SysMemSlicePitch = 0;

	byteOffset[0] = 0;
	byteStride[0] = sizeof(Vertex);

	hr = device->CreateBuffer(&vb, &vdata, m_VertexBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { VH_EXCEPT(hr); }

	// create index buffer
	D3D11_BUFFER_DESC vdesc;
	D3D11_SUBRESOURCE_DATA vidata;
	ZeroMemory(&vdesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&vidata, sizeof(D3D11_SUBRESOURCE_DATA));

	vdesc.Usage = D3D11_USAGE_DEFAULT;
	vdesc.ByteWidth = sizeof(unsigned short) * indices.size();
	vdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vdesc.CPUAccessFlags = 0;
	vdesc.MiscFlags = 0;
	vdesc.StructureByteStride = sizeof(unsigned short);
	vidata.pSysMem = static_cast<void*>(indices.data());
	vidata.SysMemPitch = 0;
	vidata.SysMemSlicePitch = 0;

	indiceCount = indices.size();

	hr = device->CreateBuffer(&vdesc, &vidata, m_IndexBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { VH_EXCEPT(hr); }

	// Create input layout
	WRL::ComPtr<ID3DBlob> vertexBlob;

	hr = D3DReadFileToBlob(L"VertexShader.cso", vertexBlob.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { VH_EXCEPT(hr); }

	// Create an vertex input layout
	D3D11_INPUT_ELEMENT_DESC IALayouts[] = {
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,   0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device->CreateInputLayout(
		IALayouts,
		2,
		reinterpret_cast<void**>(vertexBlob->GetBufferPointer()),
		vertexBlob->GetBufferSize(),
		m_InputLayout.ReleaseAndGetAddressOf()
	);
	if (FAILED(hr)) { VH_EXCEPT(hr); }

	// Bind layout to pipeline
	deviceContext->IASetInputLayout(m_InputLayout.Get());
	return;
}

VertexBuffer::~VertexBuffer(void)
{
	return;
}

unsigned int VertexBuffer::getNumIndices(void) const noexcept
{
	return indiceCount;
}