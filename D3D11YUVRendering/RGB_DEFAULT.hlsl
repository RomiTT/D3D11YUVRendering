//-----------------------------------------------------------
// YUV TO RGB BT.601 For SD TV
//-----------------------------------------------------------

cbuffer PS_CONSTANT_BUFFER : register(b0)
{
	float Opacity;
	float ignoreA;
	float ignoreB;
	float ignoreC;
};


Texture2D shaderTexture;
SamplerState SampleType;


struct PS_INPUT
{
	float4 Position   : SV_POSITION;
	float2 Texture    : TEXCOORD0;
};


float4 PS(PS_INPUT In) : SV_TARGET
{
	float4 rgba;

	rgba = shaderTexture.Sample(SampleType, In.Texture);
	rgba.a = rgba.a * Opacity;
	return rgba;
}
