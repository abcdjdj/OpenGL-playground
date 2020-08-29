#version 450
layout(location = 0) in vec3 pos;
out vec4 vCol;
uniform mat4 model;

void main()
{
    gl_Position = model * vec4(pos.x * 0.4, pos.y * 0.4, pos.z, 1.0);
    vCol = vec4(clamp(pos, 0.0, 1.0), 1.0);
}
