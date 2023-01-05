#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ProjectionView;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Transform;

uniform vec3 u_Color;
uniform int u_CustomColor;

out vec2 v_TexCoord;
out vec4 outColor;

void main()
{
	outColor = vec4(a_Normal, 1);
	if (u_CustomColor == 1)
	{
		outColor = vec4(a_Normal, 1);
	}
	v_TexCoord = a_TexCoord;
	gl_Position = u_ProjectionView * u_ViewMatrix * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 410 core

in vec2 v_TexCoord;
in vec4 outColor;

uniform sampler2D u_Texture;

out vec4 color;

void main()
{
	highp vec4 texture = texture2D(u_Texture, gl_PointCoord);
	if(texture.a < 0.5)
		discard;
    color = texture * outColor;
}