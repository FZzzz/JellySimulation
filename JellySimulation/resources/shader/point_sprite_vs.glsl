#version 410

layout (location = 0) in vec3 aPos;

uniform mat4 pvm;
uniform float point_size;
uniform vec3 light_pos;
uniform mat4 view;

out vec3 light_dir;

void main()
{
    light_dir = normalize(vec3(light_pos - vec3(view * vec4(aPos, 1.f))));
    gl_Position = pvm * vec4(aPos, 1.0f);
    gl_PointSize = point_size;
}