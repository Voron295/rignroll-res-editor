Texture2D g_Texture;

SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_OUTPUT
{
	float4 Pos : SV_Position;
	float4 Dif : COLOR;
	float2 Tex : TEXCOORD;
};

float4 PSUntex(VS_OUTPUT In) : SV_Target
{
	return In.Dif;
}