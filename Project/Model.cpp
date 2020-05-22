#include "Model.h"

//std::vector<Vertex> vertices = {
//	// FRONT FACE -- WHITE
//	{ { -0.5f, 0.0f, 0.100f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // bottom left front tri	-- 1
//	{ {  0.0f, 0.7f, 0.100f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // top of front tri		-- 2
//	{ {  0.5f, 0.0f, 0.100f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // bottom right front tri	-- 3
//
//	// LEFT FACE -- 2 TRIANGLES -- GREEN
//	{ { -0.5f, 0.0f, 0.100f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // bottom left front tri	-- 1
//	{ { -0.5f, 0.0f, 0.500f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // bottom left back tri	-- 4
//	{ {  0.0f, 0.7f, 0.100f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // top of front tri		-- 2
//
//	{ {  0.0f, 0.7f, 0.100f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // top of front tri		-- 2
//	{ {  0.0f, 0.7f, 0.500f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // top of back tri			-- 5
//	{ { -0.5f, 0.0f, 0.500f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // bottom left back tri	-- 4
//
//	// RIGHT FACE -- 2 TRIANGLES -- BLUE
//	{ {  0.5f, 0.0f, 0.100f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // bottom right front tri	-- 3
//	{ {  0.5f, 0.0f, 0.500f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // bottom right back tri	-- 6
//	{ {  0.0f, 0.7f, 0.100f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // top of front tri		-- 2
//
//	{ {  0.0f, 0.7f, 0.500f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // top of back tri			-- 5
//	{ {  0.0f, 0.7f, 0.100f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // top of front tri		-- 2
//	{ {  0.5f, 0.0f, 0.500f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // bottom right back tri	-- 6
//
//	// BACK FACE -- RED
//	{ { -0.5f, 0.0f, 0.500f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // bottom left back tri	-- 4
//	{ {  0.0f, 0.7f, 0.500f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // top of back tri			-- 5
//	{ {  0.5f, 0.0f, 0.500f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // bottom right back tri	-- 6
//};

Model::Model(void)
{
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
		
		if (mesh->mTextureCoords[0]) {
			vertex.texcoord.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
			vertex.texcoord.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
		}

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

void Model::draw(void)
{
	assert(model_has_been_initialised);

	for (auto i = 0; i < meshes.size(); i++) {
		meshes[i].draw();
	}
	return;
}