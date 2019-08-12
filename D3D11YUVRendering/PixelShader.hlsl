// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//----------------------------------------------------------------------

Texture2D tx : register( t0 );
SamplerState samLinear : register( s0 );

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};


float3 yuv_to_rgb_BT601(float3 yuv) {
	float3 rgb;
	yuv.x = 1.164383561643836 * (yuv.x - 0.0625);
	yuv.y = yuv.y - 0.5;
	yuv.z = yuv.z - 0.5;
	rgb.x = saturate(yuv.x + 1.596026785714286 * yuv.z);
	rgb.y = saturate(yuv.x - 0.812967647237771 * yuv.z - 0.391762290094914 * yuv.y);
	rgb.z = saturate(yuv.x + 2.017232142857142 * yuv.y);
}

float3 yuv_to_rgb_BT709(float3 yuv) {
	float3 rgb;
	yuv.x = 1.164383561643836 * (yuv.x - 0.0625); 
	yuv.y = yuv.y - 0.5; 
	yuv.z = yuv.z - 0.5; 
	rgb.x = saturate(yuv.x + 1.792741071428571 * yuv.z); 
	rgb.y = saturate(yuv.x - 0.532909328559444 * yuv.z - 0.21324861427373 * yuv.y); 
	rgb.z = saturate(yuv.x + 2.112401785714286 * yuv.y); 
	return rgb; 
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 yuva = tx.Sample(samLinear, input.Tex);

	return float4(yuv_to_rgb_BT709(yuva.xyz), yuva.w); // don't lose alpha
}
