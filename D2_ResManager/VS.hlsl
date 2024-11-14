struct VS_OUTPUT
{
	float4 Pos : SV_Position;
	float4 Dif : COLOR;
	float2 Tex : TEXCOORD;
};

VS_OUTPUT VS(float3 vPos : POSITION,
	float4 Dif : COLOR,
	float2 vTexCoord0 : TEXCOORD)
{
	VS_OUTPUT Output;

	Output.Pos = float4(vPos, 1.0f);
	Output.Dif = Dif;
	Output.Tex = vTexCoord0;

	return Output;
}