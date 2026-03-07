#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoords;

out vec2 voTexCoords;
out vec3 voNormal;
out vec3 voFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    voFragPos = vec3(model * vec4(pos, 1.0));
    voNormal = normal;
    voTexCoords = textureCoords + 1;
}
