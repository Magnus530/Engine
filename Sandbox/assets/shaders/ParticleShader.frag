#version 410 core

uniform sampler2D tex;

in vec4 outColor;

out vec4 vFragColor;

void main()
{
//        vFragColor = texture(tex, gl_PointCoord) * outColor;
//        vFragColor = outColor;
        vFragColor = vec4(1,0,0,1);
}


++++++++++++++