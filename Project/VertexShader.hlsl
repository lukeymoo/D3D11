cbuffer viewMatrix : register(b0)
{
	float4x4 viewmat;
};

struct vertexShaderOutput
{
	float4 position : SV_Position;
	float4 color : Color;
};

vertexShaderOutput main(float4 position : Position, float4 color : Color)
{
	vertexShaderOutput vso;

	vso.position = mul(position, viewmat);
	vso.color = color;
	return vso;
}