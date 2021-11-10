#define FRAME_BUFFER_WIDTH		1600
#define FRAME_BUFFER_HEIGHT		900

SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);

// ================================================ TITLE SHADER ================================================

struct VS_TITLE_INPUT
{
	float3		m_SPosition		 : POSITION;
	float2		m_SSize			 : SIZE;
	uint		m_SpriteRow		 : ROW;
	uint		m_SpriteColumn	 : COLUMN;
	float		m_SpriteIndex	 : INDEX;
};

struct VS_TITLE_OUTPUT
{
	float3		m_SCenter		 : POSITION;
	float2		m_SSize			 : SIZE;
	uint		m_SpriteRow		 : ROW;
	uint		m_SpriteColumn	 : COLUMN;
	float		m_SpriteIndex	 : INDEX;
};

struct GS_TITLE_OUTPUT
{
	float4		m_Position		 : SV_POSITION;
	float2		m_UV			 : TEXCOORD;
};

float3 TransCoordScreenToCamera(float Xpos, float Ypos)
{
	// 뷰포트 좌표계를 카메라 좌표계로 변환한다.
	return float3(2.0f * Xpos / FRAME_BUFFER_WIDTH - 1.0f, -2.0f * Ypos / FRAME_BUFFER_HEIGHT + 1.0f, 0.0f);
}

VS_TITLE_OUTPUT VS_Title(VS_TITLE_INPUT Input)
{
	VS_TITLE_OUTPUT Output;

	Output.m_SCenter = Input.m_SPosition;
	Output.m_SSize = Input.m_SSize;
	Output.m_SpriteRow = Input.m_SpriteRow;
	Output.m_SpriteColumn = Input.m_SpriteColumn;
	Output.m_SpriteIndex = Input.m_SpriteIndex;

	return Output;
}

[maxvertexcount(4)]
void GS_Title(point VS_TITLE_OUTPUT Input[1], inout TriangleStream<GS_TITLE_OUTPUT> OutStream)
{
	float ScreenX = Input[0].m_SCenter.x;
	float ScreenY = Input[0].m_SCenter.y;
	float HalfWidth = 0.5f * Input[0].m_SSize.x;
	float HalfHeight = 0.5f * Input[0].m_SSize.y;

	float4 Vertices[4];

	Vertices[0] = float4(TransCoordScreenToCamera(ScreenX - HalfWidth, ScreenY + HalfHeight), 1.0f);
	Vertices[1] = float4(TransCoordScreenToCamera(ScreenX - HalfWidth, ScreenY - HalfHeight), 1.0f);
	Vertices[2] = float4(TransCoordScreenToCamera(ScreenX + HalfWidth, ScreenY + HalfHeight), 1.0f);
	Vertices[3] = float4(TransCoordScreenToCamera(ScreenX + HalfWidth, ScreenY - HalfHeight), 1.0f);

	float2 UVs[4];

	UVs[0] = float2(0.0f, 1.0f);
	UVs[1] = float2(0.0f, 0.0f);
	UVs[2] = float2(1.0f, 1.0f);
	UVs[3] = float2(1.0f, 0.0f);

	float PieceWidth = 1.0f / Input[0].m_SpriteColumn;
	float PieceHeight = 1.0f / Input[0].m_SpriteRow;
	uint SpriteIndex = (uint)Input[0].m_SpriteIndex;

	for (int i = 0; i < 4; ++i)
	{
		UVs[i].x = PieceWidth * UVs[i].x + PieceWidth * (SpriteIndex % Input[0].m_SpriteColumn);
		UVs[i].y = PieceHeight * UVs[i].y + PieceHeight * (SpriteIndex / Input[0].m_SpriteColumn);
	}

	GS_TITLE_OUTPUT Output;

	for (int i = 0; i < 4; ++i)
	{
		Output.m_Position = Vertices[i];
		Output.m_UV = UVs[i];

		OutStream.Append(Output);
	}
}

float4 PS_Title(GS_TITLE_OUTPUT Input) : SV_TARGET
{
	float4 Color = Texture.Sample(Sampler, Input.m_UV);

	return Color;
}
