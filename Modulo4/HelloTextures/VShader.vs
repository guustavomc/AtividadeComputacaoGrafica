#version 450
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 tex_coord_shader;
out vec3 final_color;
out vec3 frag_pos;
out vec3 scaled_normal;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
    tex_coord_shader = vec2(tex_coord.x, 1 - tex_coord.y);
	final_color = color;
	frag_pos = vec3(model * vec4(position, 1.0));
	scaled_normal = vec3(model * vec4(normal, 1.0));
}