#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;  
layout(location = 2) in vec2 a_TexCoord;

out vec3 texCoords;

uniform mat4 u_ProjectionView;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Transform;

void main(void)
{   
    vec4 pos = u_ProjectionView * u_ViewMatrix * vec4(a_Position, 1.0f);
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
    texCoords = vec3(a_Position.x, a_Position.y, -a_Position.z);
}

#type fragment
#version 410 core

out vec4 fragmentColor;

in vec3 texCoords;
uniform samplerCube skybox;

void main(void)
{
    fragmentColor = texture(skybox, texCoords);
}