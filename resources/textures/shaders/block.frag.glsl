out vec4 Color;

in VS_OUT
{
	vec4 Color;
	vec3 TexCoord;
} fs_in;

uniform sampler2DArray u_Albedo;

uniform float u_Time;

const float PI = 3.14159265359;

#define Depth gl_FragDepth

uniform vec3 u_CameraPosition;

void main()
{
	vec4 color = texture(u_Albedo, fs_in.TexCoord);
    
	if (color.a < 0.5)
	{
		discard;
		return;
	}
	
	Color = color * fs_in.Color;
}