//-----------------------------------------------------------
// YUV TO RGB BT.709 For HD TV
//-----------------------------------------------------------

cbuffer PS_CONSTANT_BUFFER : register(b0)
{
	float Opacity;
	float ignoreA;
	float ignoreB;
	float ignoreC;
};

Texture2D shaderTextureYUYV;
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
	yuv.x = shaderTextureYUYV.Sample(SampleType, In.Texture).x;
	yuv.y = shaderTextureYUYV.Sample(SampleType, In.Texture).y;
	yuv.z = shaderTextureYUYV.Sample(SampleType, In.Texture).a;
	yuv.x = 1.164383561643836 * (yuv.x - 0.0625);
	yuv.y = yuv.y - 0.5;
	yuv.z = yuv.z - 0.5;
	rgba.x = saturate(yuv.x + 1.792741071428571 * yuv.z);
	rgba.y = saturate(yuv.x - 0.532909328559444 * yuv.z - 0.21324861427373 * yuv.y);
	rgba.z = saturate(yuv.x + 2.112401785714286 * yuv.y);
	rgba.a = Opacity;
	return rgba;
}
