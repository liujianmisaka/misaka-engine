//--------------------------
// - Hazel -
// Renderer3D Sphere Shader
// --------------------------

#type vertex
#version 450 core

layout(location = 0) in vec3 a_WorldPosition;
layout(location = 1) in vec3 a_LocalPosition;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_Radius;
layout(location = 4) in int a_EntityId;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct SphereOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Radius;
};

layout (location = 0) out SphereOutput Output;
layout (location = 3) out flat int v_EntityId;

void main()
{
	Output.LocalPosition = a_LocalPosition;
	Output.Color = a_Color;
	Output.Radius = a_Radius;

	v_EntityId = a_EntityId;

	gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityId;

struct SphereOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Radius;
};

layout (location = 0) in SphereOutput Input;
layout (location = 3) in flat int v_EntityId;

void main()
{
    // Calculate distance and fill circle with white
    float distance = 1.0 - length(Input.LocalPosition);
    float circle = smoothstep(0.0, Input.Fade, distance);
    circle *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, distance);

	if (circle == 0.0)
		discard;

    // Set output color
    o_Color = Input.Color;
	o_Color.a *= circle;

	o_EntityId = v_EntityId;
}
