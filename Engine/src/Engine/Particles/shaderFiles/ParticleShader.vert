#version 410 core

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vColor;
out vec4 outColor;
uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;

void main()
{
    outColor = vColor;
//    vec4 eyePos = mMatrix * vVertex;
//    gl_Position = pMatrix * eyePos;


//        float dist = length(eyePos.xyz);
//        float att = inversesqrt(0.1f*dist);
//        gl_PointSize = 2.0f * att;
    gl_Position = pMatrix * vMatrix * mMatrix * vVertex;
}
