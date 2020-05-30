#include "Model.h"

Model::Model(void)
	:
	modelToWorldMatrix(XMMatrixIdentity()),
	position(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f))
{
	// cannot do initialization here
	// no device or device context provided until `init()` is called
	return;
}

Model::~Model(void)
{
	return;
}

void Model::init(
	WRL::ComPtr<ID3D11Device> dV,
	WRL::ComPtr<ID3D11DeviceContext> dC,
	std::string path_to_model_data)
{
	device = dV;
	deviceContext = dC;
	model_path = path_to_model_data;
	// load model populates vertices & indices for meshes.push_back
	if (!loadModel()) {
		V_EXCEPT("Failed to load model -> " + model_path);
	}

	if (m_InputLayout != nullptr) {
		m_InputLayout.Reset();
	}

	HRESULT hr;

	// create initial world matrix for this model
	position = XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
	modelToWorldMatrix = XMMatrixTranslationFromVector(position);

	// create and load buffer
	D3D11_BUFFER_DESC md;
	D3D11_SUBRESOURCE_DATA mdd;
	ZeroMemory(&md, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&mdd, sizeof(D3D11_SUBRESOURCE_DATA));

	md.ByteWidth = sizeof(XMMATRIX);
	md.Usage = D3D11_USAGE_DYNAMIC;
	md.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	md.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	md.MiscFlags = 0;
	md.StructureByteStride = sizeof(float);
	mdd.pSysMem = &modelToWorldMatrix;

	// fill buffer with initial world matrix data
	hr = device->CreateBuffer(&md, &mdd, pWorldMatrix.GetAddressOf());
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
		m_InputLayout.GetAddressOf()
	);
	if (FAILED(hr)) { VH_EXCEPT(hr); }

	model_has_been_initialised = true;
	return;
}

bool Model::loadModel(void)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		model_path,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded
	);
	if (scene == nullptr) {
		return false;
	}
	processNode(scene->mRootNode, scene);
	return true;
}

void Model::processNode(aiNode* rootNode, const aiScene* scene)
{
	for (UINT i = 0; i < rootNode->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[rootNode->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (UINT i = 0; i < rootNode->mNumChildren; i++) {
		processNode(rootNode->mChildren[i], scene);
	}
	return;
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* pScene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;

	// get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;
		
		vertex.color.r = 0.5f;
		if (mesh->mTextureCoords[0]) {
			vertex.color.b = mesh->mTextureCoords[0][i].x;
			vertex.color.g = mesh->mTextureCoords[0][i].y;
		}
		vertex.color.a = 1.0f;

		vertices.push_back(vertex);
	}

	// get index data
	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	return Mesh(device, deviceContext, vertices, indices);
}

XMVECTOR Model::getEyePositionVector(void) const noexcept
{
	XMVECTOR eyepos = position;
	XMVECTOR heightV = XMVectorSet(0.2f, 14.2f, 1.1f, 0.0f);
	return eyepos + heightV;
}

// for now we will apply the camera rotation to the model
void Model::update(void) noexcept
{
	modelToWorldMatrix = XMMatrixTranslationFromVector(position);
	return;
}

void Model::draw(XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	assert(model_has_been_initialised);

	HRESULT hr;

	// update world matrix for use
	update();

	// apply camera view & projection matrix
	XMMATRIX modelToWorldMatrixCurrent = modelToWorldMatrix * viewMatrix * projectionMatrix;

	// Transpose it before sending to buffer
	modelToWorldMatrixCurrent = XMMatrixTranspose(modelToWorldMatrixCurrent);

	D3D11_MAPPED_SUBRESOURCE mappedMatrix;
	ZeroMemory(&mappedMatrix, sizeof(D3D11_MAPPED_SUBRESOURCE));

	// Update pWorldMatrix buffer & bind to pipeline
	hr = deviceContext->Map(pWorldMatrix.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMatrix);
	if (FAILED(hr)) { VH_EXCEPT(hr); }

	CopyMemory(mappedMatrix.pData, &modelToWorldMatrixCurrent, sizeof(XMMATRIX));

	deviceContext->Unmap(pWorldMatrix.Get(), 0);

	// bind matrix transformation buffer before drawing
	deviceContext->VSSetConstantBuffers(0, 1, pWorldMatrix.GetAddressOf());
	deviceContext->IASetPrimitiveTopology(topology);

	// bind model's input layout
	deviceContext->IASetInputLayout(m_InputLayout.Get());

	for (size_t i = 0; i < meshes.size(); i++) {

		// skip if no vertices to draw
		if (meshes[i].vertexBuffer.getNumIndices() == 0) {
			continue;
		}

		// defines vertices
		deviceContext->IASetVertexBuffers(
			0,
			1,
			meshes[i].vertexBuffer.m_VertexBuffer.GetAddressOf(),
			meshes[i].vertexBuffer.byteStride,
			meshes[i].vertexBuffer.byteOffset
		);

		// indexed vertices
		deviceContext->IASetIndexBuffer(
			meshes[i].vertexBuffer.m_IndexBuffer.Get(),
			DXGI_FORMAT_R16_UINT,
			0u
		);

		deviceContext->DrawIndexed(meshes[i].vertexBuffer.getNumIndices(), 0, 0);
	}
	return;
}


void Model::move(DIRECTION dir, double frameDt) noexcept
{
	setFrameDelta(frameDt);

	switch (dir) {
		case DIRECTION::FORWARD:
		{
			moveForward_Default();
			break;
		}
		case DIRECTION::BACKWARD:
		{
			moveBackward_Default();
			break;
		}
		case DIRECTION::LEFT:
		{
			moveLeft_Default();
			break;
		}
		case DIRECTION::RIGHT:
		{
			moveRight_Default();
			break;
		}
		case DIRECTION::UP:
		{
			moveUp_Default();
			break;
		}
		case DIRECTION::DOWN:
		{
			moveDown_Default();
			break;
		}
	}
	return;
}

/*
	Move functions called privately by `move()`
*/
void Model::moveForward_Default(void) noexcept
{
	position += DEFAULT_FORWARD_VECTOR * moveSpeed * frameDelta;
	return;
}

void Model::moveBackward_Default(void) noexcept
{
	position += DEFAULT_BACKWARD_VECTOR * moveSpeed * frameDelta;
	return;
}

void Model::moveLeft_Default(void) noexcept
{
	position += DEFAULT_LEFT_VECTOR * moveSpeed * frameDelta;
	return;
}

void Model::moveRight_Default(void) noexcept
{
	position += DEFAULT_RIGHT_VECTOR * moveSpeed * frameDelta;
	return;
}

void Model::moveUp_Default(void) noexcept
{
	position += DEFAULT_UP_VECTOR * moveSpeed * frameDelta;
	return;
}

void Model::moveDown_Default(void) noexcept
{
	position += DEFAULT_DOWN_VECTOR * moveSpeed * frameDelta;
	return;
}

void Model::rotateLeft(void) noexcept
{
	return;
}

void Model::rotateRight(void) noexcept
{
	return;
}

void Model::setTopology(D3D11_PRIMITIVE_TOPOLOGY type) noexcept
{
	topology = type;
	return;
}

void Model::setStartIndex(int index) noexcept
{
	startIndex = index;
	return;
}

void Model::setFrameDelta(double fd) noexcept
{
	frameDelta = static_cast<float>(fd);
	return;
}