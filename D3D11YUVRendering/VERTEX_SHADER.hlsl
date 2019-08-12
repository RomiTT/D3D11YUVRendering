
struct VS_INPUT
{
	float4 Position   : POSITION;
	float2 Texture    : TEXCOORD0;
};


struct VS_OUTPUT
{
	float4 Position   : SV_POSITION;
	float2 Texture    : TEXCOORD0;
};


VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Output;
	Output.Position = In.Position;
	Output.Texture = In.Texture;
	return Output;
}
