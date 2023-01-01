/*From Calvins github repository: https://github.com/opengl-tutorials/ogl/blob/master/tutorial18_billboards_and_particles/Billboard.vertexshader */
/*From Calvins github repository: https://github.com/opengl-tutorials/ogl/blob/master/tutorial18_billboards_and_particles/Billboard.fragmentshader */

#type vertex
#version 410 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 a_Vertices;

out vec2 UV;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)
uniform vec3 BillboardPos; // Position of the center of the billboard
uniform vec2 BillboardSize; // Size of the billboard, in world units (probably meters)

void main()
{
    vec3 particleCenter_wordspace = BillboardPos;

    vec3 vertexPosition_worldspace = particleCenter_wordspace + CameraRight_worldspace * a_Vertices.x * BillboardSize.x
            + CameraUp_worldspace * a_Vertices.y * BillboardSize.y;

    // Output position of the vertex
    gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

    // Or, if BillboardSize is in percentage of the screen size (1,1 for fullscreen) :
    //vertexPosition_worldspace = particleCenter_wordspace;
    //gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f); // Get the screen-space position of the particle's center
    //gl_Position /= gl_Position.w; // Here we have to do the perspective division ourselves.
    //gl_Position.xy += a_Vertices.xy * vec2(0.2, 0.05); // Move the vertex in directly screen space. No need for CameraUp/Right_worlspace here.

    // Or, if BillboardSize is in pixels :
    // Same thing, just use (ScreenSizeInPixels / BillboardSizeInPixels) instead of BillboardSizeInScreenPercentage.

    // UV of the vertex. No special space for this one.
    UV = a_Vertices.xy + vec2(0.5, 0.5);
}

#type fragment
#version 410

in vec2 UV;

// Ouput data
out vec4 color;

uniform sampler2D u_Texture;

//uniform float LifeLevel;

void main()
{
        // Output color = color of the texture at the specified UV
        color = texture( u_Texture, UV );

        // Hardcoded life level, should be in a separate texture.
//        if (UV.x < LifeLevel && UV.y > 0.3 && UV.y < 0.7 && UV.x > 0.04 )
//                color = vec4(0.2, 0.8, 0.2, 1.0); // Opaque green
}
