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
	matrix		WorldMatrix		 : packoffset(c0);

	MATERIAL	Material		 : packoffset(c4);
};

cbuffer CAMERA : register(b1)
{
	matrix		ViewMatrix		 : packoffset(c0);
	matrix		ProjectionMatrix : packoffset(c4);

	float3		CameraPosition	 : packoffset(c8);
};

SamplerState Sampler : register(s0);

Texture2D Texture : register(t0);

Texture2D TerrainBaseTexture : register(t1);
Texture2D TerrainDetailTexture0 : register(t2);
Texture2D TerrainDetailTexture1 : register(t3);
Texture2D TerrainDetailTexture2 : register(t4);
Texture2D TerrainAlphaTexture0 : register(t5);
Texture2D TerrainAlphaTexture1 : register(t6);

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

float4 Lighting(float3 Position, float3 Normal)
{
	float3 ToCamera = normalize(CameraPosition - Position);
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll(MAX_LIGHTS)] for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (Lights[i].m_IsActive)
		{
			if (Lights[i].m_Type == DIRECTIONAL_LIGHT)
			{
				Color += DirectionalLight(i, Normal, ToCamera);
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

	float4 Color = 0.4f * Lighting(Input.m_WPosition, Input.m_WNormal) + Texture.Sample(Sampler, Input.m_UV);

	return Color;
}

// ====================================== OBJECT SHADER ======================================

float4 PS_Object(VS_LIGHTING_OUTPUT Input) : SV_TARGET
{
	Input.m_WNormal = normalize(Input.m_WNormal);

	float4	Color = 0.6f * Lighting(Input.m_WPosition, Input.m_WNormal) + Texture.Sample(Sampler, Input.m_UV);

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
	float3		m_WPosition		 : POSITION;
	float3		m_WNormal		 : NORMAL;
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
		Output.m_WPosition = Vertices[i].xyz;
		Output.m_WNormal = Look;
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
	float		m_FrameTime : TIME;
};

struct VS_SPRITE_BILBOARD_OUTPUT
{
	float3		m_WCenter		 : POSITION;
	float2		m_WSize			 : SIZE;
	uint		m_SpriteRow		 : ROW;
	uint		m_SpriteColumn	 : COLUMN;
	float		m_FrameTime : TIME;
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

	if (Color.a < 0.1f)
	{
		discard;
	}

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
