layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_TexCoord;
layout (location = 2) in float a_Light;

uniform float u_Time;

const float PI = 3.14159265359;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_VP;

out VS_OUT
{
	vec4 Color;
	vec3 TexCoord;
} vs_out;

uniform mat4 u_Transform;

void main()
{
	vs_out.Color = vec4(vec3(a_Light), 1.0);
	vs_out.TexCoord = a_TexCoord;
	gl_Position = VP * u_Transform * vec4(a_Position, 1.0);
}