#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;  
layout(location = 2) in vec2 a_TexCoord;

out vec3 fragmentPosition;
out vec3 normalTransposed;
out vec2 UV;      //for textures

uniform mat4 u_ProjectionView;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Transform;

void main()
{
   fragmentPosition = vec3(u_Transform * vec4(a_Position, 1.0));    // 1.0 because it is a point
   normalTransposed = mat3(transpose(inverse(u_Transform))) * a_Normal;

   UV = a_TexCoord;      //for textures
   gl_Position = u_ProjectionView * u_ViewMatrix * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 410 core
out vec4 fragmentColor;         //The final color

in vec3 normalTransposed;   //Normal of fragment, already transposed
in vec3 fragmentPosition;
in vec2 UV;       //for textures

uniform sampler2D textureSampler;

uniform float u_AmbientStrength = 1.;
uniform vec3 u_AmbientColor = vec3(.3, .3, 1.);  // blue

uniform vec3 u_LightPosition;
uniform vec3 u_LightColor = vec3(.3, .3, 1.);  //blue
uniform float u_LightStrength = 0.3;
uniform float u_SpecularStrength = 0.2;
uniform int u_SpecularExponent = 20;

uniform vec3 u_Color = vec3(1.0, 1.0, 1.0); //white
uniform int u_CustomColor;

uniform vec3 u_CameraPosition = vec3(0, 0, 0);

void main()
{
    //ambient
    vec3 ambient = u_AmbientStrength * u_AmbientColor;

    //diffuse
    vec3 normalCorrected = normalize(normalTransposed);
    vec3 lightDirection = normalize(u_LightPosition - fragmentPosition);
    float angleFactor = max(dot(normalCorrected, lightDirection), 0.0);
    vec3 diffuse = angleFactor * u_Color * u_LightColor * u_LightStrength;

    //specular
    vec3 viewDirection = normalize(u_CameraPosition - fragmentPosition);
    float spec = 0.0;
    if (angleFactor > 0.0)     //calculations only needed if diffuse is above 0.0
    {
        vec3 reflectDirection = reflect(-lightDirection, normalCorrected);
        spec = pow(max(dot(viewDirection, reflectDirection), 0.0), u_SpecularExponent);
    }
    vec3 specular = spec * u_LightColor * u_SpecularStrength;

    vec3 result = ambient + diffuse + specular;

    fragmentColor = vec4(result, 1.0) * texture(textureSampler, UV);
    if (u_CustomColor == 1)
	{
        fragmentColor = vec4(-result, 1.0) * texture(textureSampler, UV);
    }
}