#include "Mesh.h"

Mesh::Mesh(
	WRL::ComPtr<ID3D11Device> dV,
	WRL::ComPtr<ID3D11DeviceContext> dC,
	std::vector<Vertex> vs,
	std::vector<unsigned short> is)
	: vertexBuffer(dV, dC, vs, is)
{
	device = dV;
	deviceContext = dC;
	vertices = vs;
	indices = is;
	return;
}
Mesh::~Mesh(void) {
	return;
}

Mesh::Mesh(const Mesh& mesh)
	: 
	vertexBuffer(mesh.device, mesh.deviceContext, mesh.vertices, mesh.indices)
{
	device = mesh.device;
	deviceContext = mesh.deviceContext;
	vertices = mesh.vertices;
	indices = mesh.indices;
	return;
}