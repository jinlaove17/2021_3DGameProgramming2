// ====================================== ROOT SIGNATURE ======================================

struct MATERIAL
{
	float4		m_Albedo;
	float4		m_Specular;
	float4		m_Ambient;
	float4		m_Emissive;
};

cbuffer OBJECT : register(b0)
{
	matrix		WorldMatrix					: packoffset(c0);

	MATERIAL	Material					: packoffset(c4);
};

cbuffer CAMERA : register(b1)
{
	matrix		ViewMatrix					: packoffset(c0);
	matrix		ProjectionMatrix			: packoffset(c4);

	float3		CameraPosition				: packoffset(c8);
};											
											
cbuffer GAMESCENE_INFO : register(b3)		
{											
	bool		IsTessellationActive		: packoffset(c0.x);
											
	float		TotalTime					: packoffset(c0.y);
	float		ElapsedTime					: packoffset(c0.z);
}											
											
SamplerState Sampler						: register(s0);
SamplerComparisonState ComparisonPCFShadow	: register(s1);

Texture2D Texture							: register(t0);
											
Texture2D TerrainBaseTexture				: register(t1);
Texture2D TerrainDetailTexture0				: register(t2);
Texture2D TerrainDetailTexture1				: register(t3);
Texture2D TerrainDetailTexture2				: register(t4);
Texture2D TerrainAlphaTexture0				: register(t5);
Texture2D TerrainAlphaTexture1				: register(t6);
											
Texture2D<float> DepthTexture				: register(t7);

// ====================================== LIGHTING SHADER ======================================
 
//#include "Lights.hlsl"
#define _WITH_LOCAL_VIEWER_HIGHLIGHTING

#define MAX_LIGHTS			1
#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

struct LIGHT
{
	bool					m_IsActive;

	float3					m_Position;
	float3					m_Direction;

	int						m_Type;

	float4					m_Diffuse;
	float4					m_Specular;
	float4					m_Ambient;

	float3					m_Attenuation;
	float 					m_Falloff;
	float					m_Range;
	float 					m_Theta;
	float					m_Phi;

	float					PADDING;

	float4x4				m_ToTextureMatrix;
};

cbuffer LIGHTS : register(b2)
{
	LIGHT					Lights[MAX_LIGHTS];

	float4					GlobalAmbient;
};

float4 DirectionalLight(int Index, float3 Normal, float3 ToCamera)
{
	float3 ToLight = -Lights[Index].m_Direction;
	float DiffuseFactor = dot(ToLight, Normal);
	float SpecularFactor = 0.0f;

	if (DiffuseFactor > 0.0f)
	{
		if (Material.m_Specular.a != 0.0f)
		{
#ifdef _WITH_REFLECT
			float3 Reflect = reflect(-ToLight, Normal);

			SpecularFactor = pow(max(dot(Reflect, ToCamera), 0.0f), Material.m_Specular.a);
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
			float3 Half = normalize(ToCamera + ToLight);
#else
			float3 Half = float3(0.0f, 1.0f, 0.0f);
#endif
			SpecularFactor = pow(max(dot(Half, Normal), 0.0f), Material.m_Specular.a);
#endif
		}
	}

	return (Lights[Index].m_Diffuse * DiffuseFactor * Material.m_Albedo) +
		   (Lights[Index].m_Specular * SpecularFactor * Material.m_Specular) +
	       (Lights[Index].m_Ambient * Material.m_Ambient);
}

float4 Lighting(float3 Position, float3 Normal, bool IsShadow, float4 ToTextureUV)
{
	float3 ToCamera = normalize(CameraPosition - Position);
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll(MAX_LIGHTS)]
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (Lights[i].m_IsActive)
		{
			float ShadowFactor = 1.0f;

			if (IsShadow)
			{
				ShadowFactor = DepthTexture.SampleCmpLevelZero(ComparisonPCFShadow, ToTextureUV.xy / ToTextureUV.ww, ToTextureUV.z / ToTextureUV.w).r;
			}

			if (Lights[i].m_Type == DIRECTIONAL_LIGHT)
			{
				Color += DirectionalLight(i, Normal, ToCamera) * ShadowFactor;
			}
			//else if (Lights[i].m_Type == POINT_LIGHT)
			//{
			//	Color += PointLight(i, Position, Normal, ToCamera);
			//}
			//else if (Lights[i].m_Type == SPOT_LIGHT)
			//{
			//	Color += SpotLight(i, Position, Normal, ToCamera);
			//}
		}
	}

	Color += (GlobalAmbient * Material.m_Ambient);
	Color.a = Material.m_Albedo.a;

	return Color;
}

struct VS_LIGHTING_INPUT
{
	float3		m_Position		 : POSITION;
	float3		m_Normal		 : NORMAL;
	float2		m_UV			 : TEXCOORD;
};

struct VS_LIGHTING_OUTPUT
{
	float4		m_Position		 : SV_POSITION;
	float3		m_WPosition		 : POSITION;
	float3		m_WNormal		 : NORMAL;
	float2		m_UV			 : TEXCOORD;
};

VS_LIGHTING_OUTPUT VS_Lighting(VS_LIGHTING_INPUT Input)
{
	VS_LIGHTING_OUTPUT Output;

	Output.m_WNormal = mul(Input.m_Normal, (float3x3)WorldMatrix);
	Output.m_WPosition = (float3)mul(float4(Input.m_Position, 1.0f), WorldMatrix);
	Output.m_Position = mul(mul(float4(Output.m_WPosition, 1.0f), ViewMatrix), ProjectionMatrix);
	Output.m_UV = Input.m_UV;

	return Output;
}

float4 PS_Lighting(VS_LIGHTING_OUTPUT Input) : SV_TARGET
{
	Input.m_WNormal = normalize(Input.m_WNormal);

float4 Color = 0.4f * Lighting(Input.m_WPosition, Input.m_WNormal, false, float4(0.0f, 0.0f, 0.0f, 0.0f)) + Texture.Sample(Sampler, Input.m_UV);

	return Color;
}

// ====================================== BULLET SHADER ======================================

float4 PS_Bullet(VS_LIGHTING_OUTPUT Input) : SV_TARGET
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return Color;
}

// ====================================== TERRAIN SHADER ====================================== 

struct VS_TERRAIN_INPUT
{
	float3		m_Position		 : POSITION;
	float3		m_Normal		 : NORMAL;
	float2		m_UV0			 : TEXCOORD0;
	float2		m_UV1			 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
	float4		m_Position		 : SV_POSITION;
	float3		m_WPosition		 : POSITION;
	float3		m_WNormal		 : NORMAL;
	float2		m_UV0			 : TEXCOORD0;
	float2		m_UV1			 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VS_Terrain(VS_TERRAIN_INPUT Input)
{
	VS_TERRAIN_OUTPUT Output;

	Output.m_WNormal = mul(Input.m_Normal, (float3x3)WorldMatrix);
	Output.m_WPosition = (float3)mul(float4(Input.m_Position, 1.0f), WorldMatrix);
	Output.m_Position = mul(mul(float4(Output.m_WPosition, 1.0f), ViewMatrix), ProjectionMatrix);
	Output.m_UV0 = Input.m_UV0;
	Output.m_UV1 = Input.m_UV1;

	return Output;
}

float4 PS_Terrain(VS_TERRAIN_OUTPUT Input) : SV_TARGET
{
	float4 BaseTextureColor = TerrainBaseTexture.Sample(Sampler, Input.m_UV0);

	float4 DetailTextureColor0 = TerrainDetailTexture0.Sample(Sampler, Input.m_UV1 * 2.0f);
	float4 DetailTextureColor1 = TerrainDetailTexture1.Sample(Sampler, Input.m_UV1);
	float4 DetailTextureColor2 = TerrainDetailTexture2.Sample(Sampler, Input.m_UV1);

	float AlphaTexture0 = TerrainAlphaTexture0.Sample(Sampler, Input.m_UV0).w;
	float AlphaTexture1 = TerrainAlphaTexture1.Sample(Sampler, Input.m_UV0).w;

	float4 Color = BaseTextureColor * (3.0f * DetailTextureColor0);

	Color = lerp(Color, DetailTextureColor1, 1.0f - AlphaTexture0);
	Color = lerp(Color, DetailTextureColor2, 1.0f - AlphaTexture1);

	return Color;
}

// ====================================== TESSELLATION TERRAIN SHADER ====================================== 

struct VS_TESSELLATION_TERRAIN_OUTPUT
{
	float3		m_Position		 : POSITION0;
	float3		m_WPosition		 : POSITION1;
	float3		m_WNormal		 : NORMAL;
	float2		m_UV0			 : TEXCOORD0;
	float2		m_UV1			 : TEXCOORD1;
};

struct HS_CONSTANT_OUTPUT
{
	float		m_TessEdges[4]	 : SV_TessFactor;
	float		m_TessInsides[2] : SV_InsideTessFactor;
};

struct HS_TESSELLATION_TERRAIN_OUTPUT
{
	float3		m_Position		 : POSITION;
	float2		m_UV0			 : TEXCOORD0;
	float2		m_UV1			 : TEXCOORD1;
};

struct DS_TESSELLATION_TERRAIN_OUTPUT
{
	float4		m_Position		 : SV_POSITION;
	float2		m_UV0			 : TEXCOORD0;
	float2		m_UV1			 : TEXCOORD1;
};

VS_TESSELLATION_TERRAIN_OUTPUT VS_TessellationTerrain(VS_TERRAIN_INPUT Input)
{
	VS_TESSELLATION_TERRAIN_OUTPUT Output;

	Output.m_Position = Input.m_Position;
	Output.m_WPosition = mul(Input.m_Position, (float3x3)WorldMatrix);
	Output.m_WNormal = mul(Input.m_Normal, (float3x3)WorldMatrix);
	Output.m_UV0 = Input.m_UV0;
	Output.m_UV1 = Input.m_UV1;

	return Output;
}

float CalculateTessFactor(float3 Position)
{
	float DistanceToCamera = distance(Position, CameraPosition);
	float t = saturate((DistanceToCamera - 10.0f) / (100.0f - 10.0f));

	return lerp(10.0f, 1.0f, t);
}

HS_CONSTANT_OUTPUT HS_Constant(InputPatch<VS_TESSELLATION_TERRAIN_OUTPUT, 16> Input)
{
	HS_CONSTANT_OUTPUT Output;

	if (IsTessellationActive)
	{
		float3 Edge[4];

		Edge[0] = 0.5f * (Input[0].m_WPosition + Input[12].m_WPosition);
		Edge[1] = 0.5f * (Input[0].m_WPosition + Input[3].m_WPosition);
		Edge[2] = 0.5f * (Input[3].m_WPosition + Input[15].m_WPosition);
		Edge[3] = 0.5f * (Input[12].m_WPosition + Input[15].m_WPosition);

		Output.m_TessEdges[0] = CalculateTessFactor(Edge[0]);
		Output.m_TessEdges[1] = CalculateTessFactor(Edge[1]);
		Output.m_TessEdges[2] = CalculateTessFactor(Edge[2]);
		Output.m_TessEdges[3] = CalculateTessFactor(Edge[3]);
		Output.m_TessInsides[0] = Output.m_TessInsides[1] = (Output.m_TessEdges[0] + Output.m_TessEdges[1] + Output.m_TessEdges[2] + Output.m_TessEdges[3]) / 4.0f;
	}
	else
	{
		Output.m_TessEdges[0] = 5.0f;
		Output.m_TessEdges[1] = 5.0f;
		Output.m_TessEdges[2] = 5.0f;
		Output.m_TessEdges[3] = 5.0f;
		Output.m_TessInsides[0] = Output.m_TessInsides[1] = 5.0f;
	}

	return Output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(16)]
[patchconstantfunc("HS_Constant")]
[maxtessfactor(64.0f)]
HS_TESSELLATION_TERRAIN_OUTPUT HS_TessellationTerrain(InputPatch<VS_TESSELLATION_TERRAIN_OUTPUT, 16> Input, uint i : SV_OutputControlPointID)
{
	HS_TESSELLATION_TERRAIN_OUTPUT Output;

	Output.m_Position = Input[i].m_Position;
	Output.m_UV0 = Input[i].m_UV0;
	Output.m_UV1 = Input[i].m_UV1;

	return Output;
}

float4 BernsteinCofficient(float t)
{
	float tInv = 1.0f - t;

	return float4(tInv * tInv * tInv, 3.0f * t * tInv * tInv, 3.0f * t * t * tInv, t * t * t);
}

float3 CubicBezierSum4x4(OutputPatch<HS_TESSELLATION_TERRAIN_OUTPUT, 16> Patch, float4 u, float4 v)
{
	float3 Sum = float3(0.0f, 0.0f, 0.0f);

	Sum += v.x * (u.x * Patch[0].m_Position + u.y * Patch[1].m_Position + u.z * Patch[2].m_Position + u.w * Patch[3].m_Position);
	Sum += v.y * (u.x * Patch[4].m_Position + u.y * Patch[5].m_Position + u.z * Patch[6].m_Position + u.w * Patch[7].m_Position);
	Sum += v.z * (u.x * Patch[8].m_Position + u.y * Patch[9].m_Position + u.z * Patch[10].m_Position + u.w * Patch[11].m_Position);
	Sum += v.w * (u.x * Patch[12].m_Position + u.y * Patch[13].m_Position + u.z * Patch[14].m_Position + u.w * Patch[15].m_Position);

	return Sum;
}

[domain("quad")]
DS_TESSELLATION_TERRAIN_OUTPUT DS_TessellationTerrain(HS_CONSTANT_OUTPUT PatchConstant, float2 uv : SV_DomainLocation, OutputPatch<HS_TESSELLATION_TERRAIN_OUTPUT, 16> Patch)
{
	DS_TESSELLATION_TERRAIN_OUTPUT Output = (DS_TESSELLATION_TERRAIN_OUTPUT)0;

	float4 uB = BernsteinCofficient(uv.x);
	float4 vB = BernsteinCofficient(uv.y);
	float3 Position = CubicBezierSum4x4(Patch, uB, vB);

	Output.m_Position = mul(mul(mul(float4(Position, 1.0f), WorldMatrix), ViewMatrix), ProjectionMatrix);
	Output.m_UV0 = lerp(lerp(Patch[0].m_UV0, Patch[3].m_UV0, uv.x), lerp(Patch[12].m_UV0, Patch[15].m_UV0, uv.x), uv.y);
	Output.m_UV1 = lerp(lerp(Patch[0].m_UV1, Patch[3].m_UV1, uv.x), lerp(Patch[12].m_UV1, Patch[15].m_UV1, uv.x), uv.y);

	return Output;
}

float4 PS_TessellationTerrain(DS_TESSELLATION_TERRAIN_OUTPUT Input) : SV_TARGET
{
	float4 BaseTextureColor = TerrainBaseTexture.Sample(Sampler, Input.m_UV0);

	float4 DetailTextureColor0 = TerrainDetailTexture0.Sample(Sampler, Input.m_UV1 * 2.0f);
	float4 DetailTextureColor1 = TerrainDetailTexture1.Sample(Sampler, Input.m_UV1);
	float4 DetailTextureColor2 = TerrainDetailTexture2.Sample(Sampler, Input.m_UV1);

	float AlphaTexture0 = TerrainAlphaTexture0.Sample(Sampler, Input.m_UV0).w;
	float AlphaTexture1 = TerrainAlphaTexture1.Sample(Sampler, Input.m_UV0).w;

	float4 Color = BaseTextureColor * (3.0f * DetailTextureColor0);

	Color = lerp(Color, DetailTextureColor1, 1.0f - AlphaTexture0);
	Color = lerp(Color, DetailTextureColor2, 1.0f - AlphaTexture1);

	return Color;
}

// ====================================== BILBOARD SHADER ====================================== 

struct VS_BILBOARD_INPUT
{
	float3		m_WPosition		 : POSITION;
	float2		m_WSize			 : SIZE;
};

struct VS_BILBOARD_OUTPUT
{
	float3		m_WCenter		 : POSITION;
	float2		m_WSize			 : SIZE;
};

struct GS_BILBOARD_OUTPUT
{
	float4		m_Position		 : SV_POSITION;
	float2		m_UV0			 : TEXCOORD0;
};

VS_BILBOARD_OUTPUT VS_Bilboard(VS_BILBOARD_INPUT Input)
{
	VS_BILBOARD_OUTPUT Output;

	Output.m_WCenter = Input.m_WPosition;
	Output.m_WSize = Input.m_WSize;

	return Output;
}

[maxvertexcount(4)]
void GS_Bilboard(point VS_BILBOARD_OUTPUT Input[1], inout TriangleStream<GS_BILBOARD_OUTPUT> OutStream)
{
	float3 Up = float3(0.0f, 1.0f, 0.0f);
	float3 Look = normalize(CameraPosition - Input[0].m_WCenter);
	float3 Right = cross(Up, Look);

	float HalfWidth = 0.5f * Input[0].m_WSize.x;
	float HalfHeight = 0.5f * Input[0].m_WSize.y;

	float4 Vertices[4];

	Vertices[0] = float4(Input[0].m_WCenter + HalfWidth * Right - HalfHeight * Up, 1.0f);
	Vertices[1] = float4(Input[0].m_WCenter + HalfWidth * Right + HalfHeight * Up, 1.0f);
	Vertices[2] = float4(Input[0].m_WCenter - HalfWidth * Right - HalfHeight * Up, 1.0f);
	Vertices[3] = float4(Input[0].m_WCenter - HalfWidth * Right + HalfHeight * Up, 1.0f);

	float2 UVs[4];

	UVs[0] = float2(0.0f, 1.0f);
	UVs[1] = float2(0.0f, 0.0f);
	UVs[2] = float2(1.0f, 1.0f);
	UVs[3] = float2(1.0f, 0.0f);

	GS_BILBOARD_OUTPUT Output;

	for (int i = 0; i < 4; ++i)
	{
		Output.m_Position = mul(mul(Vertices[i], ViewMatrix), ProjectionMatrix);
		Output.m_UV0 = UVs[i];

		OutStream.Append(Output);
	}
}

float4 PS_Bilboard(GS_BILBOARD_OUTPUT Input) : SV_TARGET
{
	float4 Color = Texture.Sample(Sampler, Input.m_UV0);

	return Color;
}

// ====================================== SPRITE BILBOARD SHADER ======================================

struct VS_SPRITE_BILBOARD_INPUT
{
	float3		m_WPosition		 : POSITION;
	float2		m_WSize			 : SIZE;
	uint		m_SpriteRow		 : ROW;
	uint		m_SpriteColumn	 : COLUMN;
	float		m_FrameTime		 : TIME;
};

struct VS_SPRITE_BILBOARD_OUTPUT
{
	float3		m_WCenter		 : POSITION;
	float2		m_WSize			 : SIZE;
	uint		m_SpriteRow		 : ROW;
	uint		m_SpriteColumn	 : COLUMN;
	float		m_FrameTime		 : TIME;
};

struct GS_SPRITE_BILBOARD_OUTPUT
{
	float4		m_Position		 : SV_POSITION;
	float2		m_UV0			 : TEXCOORD0;
};

VS_SPRITE_BILBOARD_OUTPUT VS_SpriteBilboard(VS_SPRITE_BILBOARD_INPUT Input)
{
	VS_SPRITE_BILBOARD_OUTPUT Output;

	Output.m_WCenter = Input.m_WPosition;
	Output.m_WSize = Input.m_WSize;
	Output.m_SpriteRow = Input.m_SpriteRow;
	Output.m_SpriteColumn = Input.m_SpriteColumn;
	Output.m_FrameTime = Input.m_FrameTime;

	return Output;
}

[maxvertexcount(4)]
void GS_SpriteBilboard(point VS_SPRITE_BILBOARD_OUTPUT Input[1], inout TriangleStream<GS_SPRITE_BILBOARD_OUTPUT> OutStream)
{
	float3 Up = float3(0.0f, 1.0f, 0.0f);
	float3 Look = normalize(CameraPosition - Input[0].m_WCenter);
	float3 Right = cross(Up, Look);

	float HalfWidth = 0.5f * Input[0].m_WSize.x;
	float HalfHeight = 0.5f * Input[0].m_WSize.y;

	float4 Vertices[4];

	Vertices[0] = float4(Input[0].m_WCenter + HalfWidth * Right - HalfHeight * Up, 1.0f);
	Vertices[1] = float4(Input[0].m_WCenter + HalfWidth * Right + HalfHeight * Up, 1.0f);
	Vertices[2] = float4(Input[0].m_WCenter - HalfWidth * Right - HalfHeight * Up, 1.0f);
	Vertices[3] = float4(Input[0].m_WCenter - HalfWidth * Right + HalfHeight * Up, 1.0f);

	float2 UVs[4];

	UVs[0] = float2(0.0f, 1.0f);
	UVs[1] = float2(0.0f, 0.0f);
	UVs[2] = float2(1.0f, 1.0f);
	UVs[3] = float2(1.0f, 0.0f);

	float FrameWidth = 1.0f / Input[0].m_SpriteColumn;
	float FrameHeight = 1.0f / Input[0].m_SpriteRow;
	uint FrameIndex = Input[0].m_FrameTime % (Input[0].m_SpriteRow * Input[0].m_SpriteColumn);

	for (int i = 0; i < 4; ++i)
	{
		UVs[i].x = FrameWidth * UVs[i].x + FrameWidth * (FrameIndex % Input[0].m_SpriteColumn);
		UVs[i].y = FrameHeight * UVs[i].y + FrameHeight * (FrameIndex / Input[0].m_SpriteColumn);
	}

	GS_SPRITE_BILBOARD_OUTPUT Output;

	for (int j = 0; j < 4; ++j)
	{
		Output.m_Position = mul(mul(Vertices[j], ViewMatrix), ProjectionMatrix);
		Output.m_UV0 = UVs[j];

		OutStream.Append(Output);
	}
}

float4 PS_SpriteBilboard(GS_SPRITE_BILBOARD_OUTPUT Input) : SV_TARGET
{
	float4 Color = Texture.Sample(Sampler, Input.m_UV0);

	return Color;
}

// ====================================== SKYBOX SHADER ====================================== 

struct VS_SKYBOX_INPUT
{
	float3		m_Position		 : POSITION;
	float2		m_WSize			 : SIZE;
};

struct VS_SKYBOX_OUTPUT
{
	float3		m_WCenter		 : POSITION;
	float2		m_WSize			 : SIZE;
};

struct GS_SKYBOX_OUTPUT
{
	float4		m_Position		 : SV_POSITION;
	float2		m_UV0			 : TEXCOORD0;
};

VS_SKYBOX_OUTPUT VS_SkyBox(VS_SKYBOX_INPUT Input)
{
	VS_SKYBOX_OUTPUT Output;

	float4x4 CameraWorldMatrix = { 1.0f, 0.0f, 0.0f, 0.0f,
								   0.0f, 1.0f, 0.0f, 0.0f,
								   0.0f, 0.0f, 1.0f, 0.0f,
									CameraPosition,  1.0f };

	Output.m_WCenter = (float3)mul(float4(Input.m_Position, 1.0f), CameraWorldMatrix);
	Output.m_WSize = Input.m_WSize;

	return Output;
}

[maxvertexcount(4)]
void GS_SkyBox(point VS_SKYBOX_OUTPUT Input[1], inout TriangleStream<GS_SKYBOX_OUTPUT> OutStream)
{
	float3 Look = normalize(CameraPosition - Input[0].m_WCenter);
	float3 Up = float3(0.0f, 1.0f, 0.0f);

	if (Look.y < 0.0f)
	{
		Up = float3(0.0f, 0.0f, -1.0f);
	}
	else if (Look.y > 0.0f)
	{
		Up = float3(0.0f, 0.0f, +1.0f);
	}

	float3 Right = cross(Up, Look);

	float HalfWidth = 0.5f * Input[0].m_WSize.x;
	float HalfHeight = 0.5f * Input[0].m_WSize.y;

	float4 Vertices[4];

	Vertices[0] = float4(Input[0].m_WCenter + HalfWidth * Right - HalfHeight * Up, 1.0f);
	Vertices[1] = float4(Input[0].m_WCenter + HalfWidth * Right + HalfHeight * Up, 1.0f);
	Vertices[2] = float4(Input[0].m_WCenter - HalfWidth * Right - HalfHeight * Up, 1.0f);
	Vertices[3] = float4(Input[0].m_WCenter - HalfWidth * Right + HalfHeight * Up, 1.0f);

	float2 UVs[4];

	UVs[0] = float2(0.0f, 1.0f);
	UVs[1] = float2(0.0f, 0.0f);
	UVs[2] = float2(1.0f, 1.0f);
	UVs[3] = float2(1.0f, 0.0f);

	GS_SKYBOX_OUTPUT Output;

	for (int i = 0; i < 4; ++i)
	{
		Output.m_Position = mul(mul(Vertices[i], ViewMatrix), ProjectionMatrix);
		Output.m_UV0 = UVs[i];

		OutStream.Append(Output);
	}
}

float4 PS_SkyBox(GS_SKYBOX_OUTPUT Input) : SV_TARGET
{
	float4 Color = Texture.Sample(Sampler, Input.m_UV0);

	return Color;
}

// ====================================== HP BAR SHADER ======================================

struct GS_HPBAR_OUTPUT
{
	float4		m_Position		 : SV_POSITION;
	float4		m_Color			 : COLOR;
};

static const float HpBarHalfLength = 2.25f;

[maxvertexcount(8)]
void GS_HpBar(point VS_BILBOARD_OUTPUT Input[1], inout TriangleStream<GS_HPBAR_OUTPUT> OutStream)
{
	float3 Up = float3(0.0f, 1.0f, 0.0f);
	float3 Look = normalize(CameraPosition - Input[0].m_WCenter);
	float3 Right = cross(Up, Look);

	float HalfWidth = 0.5f * Input[0].m_WSize.x;
	float HalfHeight = 0.5f * Input[0].m_WSize.y;

	float4 Vertices[8];

	Vertices[0] = float4(Input[0].m_WCenter + HalfWidth * Right + (HpBarHalfLength - HalfWidth) * Right - HalfHeight * Up + 0.001f * Look, 1.0f);
	Vertices[1] = float4(Input[0].m_WCenter + HalfWidth * Right + (HpBarHalfLength - HalfWidth) * Right + HalfHeight * Up + 0.001f * Look, 1.0f);
	Vertices[2] = float4(Input[0].m_WCenter - HalfWidth * Right + (HpBarHalfLength - HalfWidth) * Right - HalfHeight * Up + 0.001f * Look, 1.0f);
	Vertices[3] = float4(Input[0].m_WCenter - HalfWidth * Right + (HpBarHalfLength - HalfWidth) * Right + HalfHeight * Up + 0.001f * Look, 1.0f);

	Vertices[4] = float4(Input[0].m_WCenter + 1.05f * HpBarHalfLength * Right - 1.2f * HalfHeight * Up, 1.0f);
	Vertices[5] = float4(Input[0].m_WCenter + 1.05f * HpBarHalfLength * Right + 1.2f * HalfHeight * Up, 1.0f);
	Vertices[6] = float4(Input[0].m_WCenter - 1.05f * HpBarHalfLength * Right - 1.2f * HalfHeight * Up, 1.0f);
	Vertices[7] = float4(Input[0].m_WCenter - 1.05f * HpBarHalfLength * Right + 1.2f * HalfHeight * Up, 1.0f);

	GS_HPBAR_OUTPUT Output;

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			Output.m_Position = mul(mul(Vertices[4 * i + j], ViewMatrix), ProjectionMatrix);

			if (i == 0)
			{
				Output.m_Color = float4(1.0f - 0.5f * HalfWidth / HpBarHalfLength, HalfWidth / HpBarHalfLength, 0.0f, 1.0f);
			}
			else
			{
				Output.m_Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
			}

			OutStream.Append(Output);
		}
		OutStream.RestartStrip();
	}
}

float4 PS_HpBar(GS_HPBAR_OUTPUT Input) : SV_TARGET
{
	return Input.m_Color;
}

// ====================================== PARTICLE SHADER ======================================

struct VS_PARTICLE_INPUT
{
	float3		m_WPosition		 : POSITION;
	float3		m_Direction		 : DIRECTION;
	float2		m_Size		     : SIZE;
	int			m_Level			 : LEVEL;
};

VS_PARTICLE_INPUT VS_Particle(VS_PARTICLE_INPUT Input)
{
	return Input;
}

void GetPositions(float3 Origin, out float3 Positions[8])
{
	Positions[0] = Origin + float3(-15.0f, +5.0f, +15.0f);
	Positions[1] = Origin + float3(0.0f, -2.5f, +15.0f);
	Positions[2] = Origin + float3(+15.0f, -7.0f, +15.0f);
	Positions[3] = Origin + float3(-15.0f, 0.0f, 0.0f);
	Positions[4] = Origin + float3(+15.0f, -1.0f, 0.0f);
	Positions[5] = Origin + float3(-15.0f, 6.0f, -15.0f);
	Positions[6] = Origin + float3(0.0f, 0.0f, -15.0f);
	Positions[7] = Origin + float3(+15.0f, -5.0f, -15.0f);
}

[maxvertexcount(9)]
void GS_ParticleStreamOutput(point VS_PARTICLE_INPUT Input[1], inout PointStream<VS_PARTICLE_INPUT> OutStream)
{
	VS_PARTICLE_INPUT Particle = Input[0];

	if (Particle.m_Level <= 2)
	{
		VS_PARTICLE_INPUT NewParticle = (VS_PARTICLE_INPUT)0;

		float3 NearPositions[8];

		GetPositions(Particle.m_WPosition, NearPositions);

		for (int i = 0; i < 8; ++i)
		{
			NewParticle.m_WPosition = NearPositions[i];
			NewParticle.m_Direction = float3(0.0f, -1.0f * ((7 - i) % 8 + 1), 0.0f);
			NewParticle.m_Size = Particle.m_Size;
			NewParticle.m_Level = Particle.m_Level + 1;

			OutStream.Append(NewParticle);
		}

		Particle.m_Level += 1;
	}

	Particle.m_WPosition += 10.0f * Particle.m_Direction * ElapsedTime;

	if (Particle.m_WPosition.y <= 50.0f)
	{
		Particle.m_WPosition.y = 120.0f;
	}

	OutStream.Append(Particle);
}

struct GS_PARTICLE_OUTPUT
{
	float4		m_Position		 : SV_POSITION;
	float2		m_UV0			 : TEXCOORD;
};

void GetBilboardCorners(float3 Origin, float2 HalfSize, out float4 Positions[4])
{
	float3 Up = float3(0.0f, 1.0f, 0.0f);
	float3 Look = normalize(CameraPosition - Origin);
	float3 Right = normalize(cross(Up, Look));

	Positions[0] = float4(Origin + HalfSize.x * Right - HalfSize.y * Up, 1.0f);
	Positions[1] = float4(Origin + HalfSize.x * Right + HalfSize.y * Up, 1.0f);
	Positions[2] = float4(Origin - HalfSize.x * Right - HalfSize.y * Up, 1.0f);
	Positions[3] = float4(Origin - HalfSize.x * Right + HalfSize.y * Up, 1.0f);
}

static float2 BasicUVs[4] = { float2(0.0f, 1.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(1.0f, 0.0f) };

[maxvertexcount(4)]
void GS_ParticleDraw(point VS_PARTICLE_INPUT Input[1], inout TriangleStream<GS_PARTICLE_OUTPUT> OutStream)
{
	float4 Vertices[4];

	GetBilboardCorners(Input[0].m_WPosition, 0.5f * Input[0].m_Size, Vertices);

	GS_PARTICLE_OUTPUT Output = (GS_PARTICLE_OUTPUT)0;

	for (int i = 0; i < 4; ++i)
	{
		Output.m_Position = mul(mul(Vertices[i], ViewMatrix), ProjectionMatrix);
		Output.m_UV0 = BasicUVs[i];

		OutStream.Append(Output);
	}
}

float4 PS_ParticleDraw(GS_PARTICLE_OUTPUT Input) : SV_TARGET
{
	float4 Color = Texture.Sample(Sampler, Input.m_UV0);

	return Color;
}

// ====================================== DEPTH RENDER SHADER ======================================

struct PS_DEPTH_OUTPUT
{
	float		m_Position		: SV_Target;
	float		m_Depth			: SV_Depth;
};

PS_DEPTH_OUTPUT PS_DepthWriteShader(VS_LIGHTING_OUTPUT Input)
{
	PS_DEPTH_OUTPUT Output;

	Output.m_Position = Output.m_Depth = Input.m_Position.z;

	return Output;
}

// ====================================== SHADOW MAP SHADER ======================================

struct VS_SHADOW_MAP_OUTPUT
{
	float4		 m_Position		: SV_POSITION;
	float3		 m_WPosition	: POSITION;
	float3		 m_WNormal		: NORMAL;
	float4		 m_UV0			: TEXCOORD;
};

VS_SHADOW_MAP_OUTPUT VS_ShadowMap(VS_LIGHTING_INPUT Input)
{
	VS_SHADOW_MAP_OUTPUT Output = (VS_SHADOW_MAP_OUTPUT)0;

	float4 WPosition = mul(float4(Input.m_Position, 1.0f), WorldMatrix);

	Output.m_WPosition = WPosition.xyz;
	Output.m_Position = mul(mul(WPosition, ViewMatrix), ProjectionMatrix);
	Output.m_WNormal = mul(float4(Input.m_Normal, 0.0f), WorldMatrix).xyz;
	Output.m_UV0 = mul(WPosition, Lights[0].m_ToTextureMatrix);

	return Output;
}

float4 PS_ShadowMap(VS_SHADOW_MAP_OUTPUT Input) : SV_TARGET
{
	float4 Illumination = Lighting(Input.m_WPosition, normalize(Input.m_WNormal), true, Input.m_UV0);

	return Illumination;
}