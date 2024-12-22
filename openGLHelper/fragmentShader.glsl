#version 330 core

in vec4 col;
out vec4 outputColor;

void main()
{
    outputColor = vec4(abs(col.rgb), 1.0);
}
