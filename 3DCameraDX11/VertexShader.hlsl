struct VOut
{
	float4 position : SV_POSITION;
	float2 tex_coord : TEXCOORD;
};

VOut main( float4 pos : POSITION, float2 texcoord : TEXCOORD)
{
	VOut output;

	output.position = pos;
	output.tex_coord = texcoord;    // set the texture coordinates, unmodified

	return output;
}
