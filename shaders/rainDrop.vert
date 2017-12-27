#version 330

layout(location = 0) in vec3 vertexPosition;

out vec3 colour;

void main()
{
    colour = vec3(0.5f, 0.5f, 0.5f);
    gl_Position = vec4(vertexPosition, 1.0);
}
