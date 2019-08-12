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
	rgba.x = saturate(yuv.x + 1.596026785714286 * yuv.z);
	rgba.y = saturate(yuv.x - 0.812967647237771 * yuv.z - 0.391762290094914 * yuv.y);
	rgba.z = saturate(yuv.x + 2.017232142857142 * yuv.y);
	rgba.a = Opacity;
	return rgba;
}

