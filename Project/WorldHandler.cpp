#include "WorldHandler.h"

struct int24 {
	unsigned int : 24;
};

WorldHandler::WorldHandler(
	WRL::ComPtr<ID3D11Device> dV,
	WRL::ComPtr<ID3D11DeviceContext> dC
	)
{
	if (pDevice != nullptr) {
		pDevice.Reset();
	}
	if (pDeviceContext != nullptr) {
		pDeviceContext.Reset();
	}
	if (pWorldMatrix != nullptr) {
		pWorldMatrix.Reset();
	}

	pDevice = dV;
	pDeviceContext = dC;

	cbMatrix = XMMatrixIdentity();

	terrainWidth = 0;
	terrainHeight = 0;

	// create a buffer for mvp matrix data
	HRESULT hr;
	D3D11_BUFFER_DESC cb;
	D3D11_SUBRESOURCE_DATA cbd;
	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&cbd, sizeof(D3D11_SUBRESOURCE_DATA));

	cb.ByteWidth = sizeof(XMMATRIX);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = sizeof(float);
	cbd.pSysMem = &cbMatrix;

	hr = pDevice->CreateBuffer(&cb, &cbd, pWorldMatrix.GetAddressOf());
	if (FAILED(hr)) { WORLDH_EXCEPT(hr); }

	return;
}

WorldHandler::~WorldHandler(void)
{
	return;
}

DWORD WorldHandler::getWidth(void) const noexcept
{
	return terrainWidth;
}

DWORD WorldHandler::getHeight(void) const noexcept
{
	return terrainHeight;
}

bool WorldHandler::load(std::string filename)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	ZeroMemory(&fileHeader, sizeof(BITMAPFILEHEADER));
	ZeroMemory(&infoHeader, sizeof(BITMAPINFOHEADER));

	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		WORLD_EXCEPT("Failed to load specified height map -> " + filename);
		return false;
	}

	file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BITMAPFILEHEADER));
	file.read(reinterpret_cast<char*>(&infoHeader), sizeof(BITMAPINFOHEADER));

	if (fileHeader.bfType != 0x4d42) {
		WORLD_EXCEPT("File/Info header data failed assertions! Is this a BMP image? -> " + filename);
		return false;
	}

	terrainWidth = infoHeader.biWidth;
	terrainHeight = infoHeader.biHeight;

	// this will store bitmap data
	BYTE* rawData = new BYTE[3 * terrainWidth * terrainHeight];

	file.read(reinterpret_cast<char*>(rawData), (3 * terrainWidth * terrainHeight));

	if(!rawData) {
		WORLD_EXCEPT("Failed to read height map!");
	}

	file.close();

	// this will store terrain data
	terrainData = new tdPoint[terrainWidth * terrainHeight];

	int offset = 0;

	for (int y = 0; y < terrainHeight; y++) {
		for (int x = 0; x < terrainWidth; x++, offset +=3) {
			int p_Height = rawData[offset]; // `height` from heightmap
			int index = (terrainWidth * y) + x;
			
			// zero out before fetching grayscale data
			terrainData[index] = { 0, 0, 0 };

			terrainData[index].x = (float)x;
			terrainData[index].y = (float)p_Height / 10.0f;
			terrainData[index].z = (float)y;
		}
	}

	delete[] rawData;
	rawData = 0;

	store();
	return true;
}

void WorldHandler::normalize(void)
{
	for (int y = 0; y < terrainHeight; y++) {
		for (int x = 0; x < terrainWidth; x++) {
			terrainData[y * terrainHeight + x].y /= 15.0f;
		}
	}
	return;
}

void WorldHandler::store(void)
{
	int vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 12;
	int index1, index2, index3, index4;
	pVertices = std::make_unique<std::vector<Vertex>>(vertexCount);


	int index = 0;

	for (int y = 0; y < (terrainHeight - 1); y++) {
		for (int x = 0; x < (terrainWidth - 1); x++) {
			index1 = (terrainHeight * y) + x; // top left
			index2 = (terrainHeight * y) + (x + 1); // top right
			index3 = (terrainHeight * (y + 1)) + x; // bottom left
			index4 = (terrainHeight * (y + 1)) + (x + 1); // bottom right
			Vertex local;

			// ----------
			// top left
			local.position = Position(terrainData[index1].x, terrainData[index1].y, terrainData[index1].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;

			// bottom left
			local.position = Position(terrainData[index3].x, terrainData[index3].y, terrainData[index3].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;
			// ----------

			// ----------
			// bottom left
			local.position = Position(terrainData[index3].x, terrainData[index3].y, terrainData[index3].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;

			// bottom right
			local.position = Position(terrainData[index4].x, terrainData[index4].y, terrainData[index4].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;
			// ----------


			// ----------
			// bottom right
			local.position = Position(terrainData[index4].x, terrainData[index4].y, terrainData[index4].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;
			
			// top left
			local.position = Position(terrainData[index1].x, terrainData[index1].y, terrainData[index1].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;
			// ----------







			
			// ----------
			// top left
			local.position = Position(terrainData[index1].x, terrainData[index1].y, terrainData[index1].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;

			// top right
			local.position = Position(terrainData[index2].x, terrainData[index2].y, terrainData[index2].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;
			// ----------

			// ----------
			// top right
			local.position = Position(terrainData[index2].x, terrainData[index2].y, terrainData[index2].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;

			// bottom right
			local.position = Position(terrainData[index4].x, terrainData[index4].y, terrainData[index4].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;
			// ----------


			// ----------
			// bottom right
			local.position = Position(terrainData[index4].x, terrainData[index4].y, terrainData[index4].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;

			// top left
			local.position = Position(terrainData[index1].x, terrainData[index1].y, terrainData[index1].z);
			local.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			pVertices->at(index) = local;
			index++;
			// ----------

		}
	}

	// create vertexBuffer
	vertexBuffer = std::make_unique<VertexBuffer>(pDevice, pDeviceContext, pVertices.get());

	// delete the terrainData array
	delete[] terrainData;

	heightmap_has_been_loaded = true;
	return;
}

void WorldHandler::prepare(void)
{
	cbMatrix = XMMatrixTranslationFromVector(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	return;
}

void WorldHandler::draw(XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	assert(heightmap_has_been_loaded);

	HRESULT hr;

	// update cbMatrix data
	prepare();

	// no world matrix because all vertices are already in world coords
	XMMATRIX finalMatrix = cbMatrix;
	finalMatrix *= viewMatrix * projectionMatrix;

	// transpose before updating mvp buffer
	finalMatrix = XMMatrixTranspose(finalMatrix);

	// update pipeline buffer & BIND
	D3D11_MAPPED_SUBRESOURCE mp;
	ZeroMemory(&mp, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = pDeviceContext->Map(pWorldMatrix.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mp);
	if (FAILED(hr)) { WORLDH_EXCEPT(hr); }

	CopyMemory(mp.pData, &finalMatrix, sizeof(XMMATRIX));

	pDeviceContext->Unmap(pWorldMatrix.Get(), 0);

	pDeviceContext->VSSetConstantBuffers(0, 1, pWorldMatrix.GetAddressOf());

	// bind vertex buffer
	pDeviceContext->IASetVertexBuffers(
		0,
		1,
		vertexBuffer->m_VertexBuffer.GetAddressOf(),
		vertexBuffer->byteStride,
		vertexBuffer->byteOffset
	);

	// set topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// call draw
	pDeviceContext->Draw(pVertices->size(), 0);

	return;
}


/*
	WORLDHANDLER EXCEPTIONS
*/

WorldHandler::Exception::Exception(int l, const char* f, HRESULT h)
	: ExceptionHandler(l, f, h)
{
	type = "World Handler Exception";
	return;
}

WorldHandler::Exception::Exception(std::string m, int l, const char* f)
	: ExceptionHandler(l, f, 0)
{
	type = "World Handler Exception";
	errorDescription = m;
	return;
}

WorldHandler::Exception::~Exception(void)
{
	return;
}