#version 330

in vec3 vertexPosition;

uniform mat4 matProjection;
uniform mat4 matView;

out vec3 fragPosition;

void main()
{
    fragPosition = vertexPosition;
    gl_Position = matProjection * mat4(mat3(matView)) * vec4(vertexPosition, 1.0);
}