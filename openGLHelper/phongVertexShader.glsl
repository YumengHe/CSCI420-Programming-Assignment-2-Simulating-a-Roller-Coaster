#version 330

layout(location = 0) in vec3 position; // vertex position in world space
layout(location = 1) in vec3 normal;   // vertex normal in world space

out vec3 viewPosition; // vertex position in view space
out vec3 viewNormal;   // vertex normal in view space

uniform mat4 modelViewMatrix;  // transformation matrix
uniform mat4 normalMatrix;     // transformation matrix
uniform mat4 projectionMatrix; // transformation matrix

void main()
{
  // view-space position of the vertex
  vec4 viewPosition4 = modelViewMatrix * vec4(position, 1.0f);
  viewPosition = viewPosition4.xyz;

  // final position in the normalized device coordinates space
  gl_Position = projectionMatrix * viewPosition4;

  // view-space normal
  viewNormal = normalize((normalMatrix * vec4(normal, 0.0f)).xyz);
}
