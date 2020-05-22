struct vertexShaderOutput
{
	float4 position : SV_Position;
	float4 color : Color;
};

float4 main(vertexShaderOutput vso) : SV_Target
{
	return vso.color;
}