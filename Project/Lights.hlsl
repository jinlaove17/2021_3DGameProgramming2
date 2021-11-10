#define _WITH_LOCAL_VIEWER_HIGHLIGHTING

#define MAX_LIGHTS			1
#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

struct LIGHT
{
	bool					m_IsActive;

	int						m_Type;

	float3					m_Position;
	float3					m_Direction;

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
