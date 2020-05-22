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

void Mesh::draw(void)
{
	// check if there's any data to draw
	if (vertexBuffer.getNumIndices() == 0) {
		return;
	}
	deviceContext->IASetInputLayout(vertexBuffer.m_InputLayout.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// defines vertices
	deviceContext->IASetVertexBuffers(
		0,
		1,
		vertexBuffer.m_VertexBuffer.GetAddressOf(),
		vertexBuffer.byteStride,
		vertexBuffer.byteOffset
	);

	// indexed vertices
	deviceContext->IASetIndexBuffer(
		vertexBuffer.m_IndexBuffer.Get(),
		DXGI_FORMAT_R16_UINT,
		0u
	);

	deviceContext->DrawIndexed(vertexBuffer.getNumIndices(), 0, 0);
	return;
}