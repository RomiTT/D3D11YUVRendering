//-----------------------------------------------------------
// YUV TO RGB BT.2020 For UHD TV
//-----------------------------------------------------------

cbuffer PS_CONSTANT_BUFFER : register(b0)
{
	float Opacity;
	float ignoreA;
	float ignoreB;
	float ignoreC;
};

Texture2D shaderTextureY;
Texture2D shaderTextureUV;
SamplerState SampleType;

struct PS_INPUT
{
	float4 Position   : SV_POSITION;
	float2 Texture    : TEXCOORD0;
};

float4 PS(PS_INPUT In) : SV_TARGET
{
	float3 yuv;
	float4 rgba;
	yuv.x = shaderTextureY.Sample(SampleType, In.Texture).x;
	yuv.yz = shaderTextureUV.Sample(SampleType, In.Texture).xy;
	yuv.x = 1.164383561643836 * (yuv.x - 0.0625);
	yuv.y = yuv.y - 0.5;
	yuv.z = yuv.z - 0.5;
	rgba.x = yuv.x + 1.792741071428571 * yuv.z;
	rgba.y = yuv.x - 0.532909328559444 * yuv.z - 0.21324861427373 * yuv.y;
	rgba.z = yuv.x + 2.112401785714286 * yuv.y;
	rgba.x = saturate(1.661 * rgba.x - 0.588 * rgba.y - 0.073 * rgba.z);
	rgba.y = saturate(-0.125 * rgba.x + 1.133 * rgba.y - 0.008 * rgba.z);
	rgba.z = saturate(-0.018 * rgba.x - 0.101 * rgba.y + 1.119 * rgba.z);
	rgba.a = Opacity;
	return rgba;
}
