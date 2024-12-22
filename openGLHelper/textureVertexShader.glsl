#version 330 core

layout(location = 0) in vec3 position; // Vertex position
layout(location = 1) in vec2 texCoord; // Texture coordinate

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec2 TexCoord;

void main()
{
  gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
  TexCoord = texCoord;
}
