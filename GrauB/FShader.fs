#version 450

in vec2 tex_coord_shader;
in vec3 final_color;
in vec3 frag_pos;
in vec3 scaled_normal;

out vec4 color;

uniform sampler2D tex_buffer;

//Propriedades da superficie
uniform float ka;
uniform float kd;
uniform float ks;
uniform float q;

//Propriedades da fonte de luz
uniform vec3 light_pos;
uniform vec3 light_color;

//Posição da Camera
uniform vec3 camera_pos;

void main()
{
	//color = final_color;
	//color = texture(tex_buffer, tex_coord_shader);

	//Cálculo da parcela de iluminação ambiente
	vec3 ambient = ka * light_color;

	//Cálculo da parcela de iluminação difusa
	vec3 N = normalize(scaled_normal);
	vec3 L = normalize(light_pos - frag_pos);
	float diff = max(dot(N,L),0.0);
	vec3 diffuse = kd * diff * light_color;

	//Cálculo da parcela de iluminação especular
	vec3 V = normalize(camera_pos - frag_pos);
	vec3 R = normalize(reflect(-L,N));
	float spec = max(dot(R,V),0.0);
	spec = pow(spec,q);
	vec3 specular = ks * spec * light_color;

	vec3 result = (ambient + diffuse) * vec3(texture(tex_buffer, tex_coord_shader)) + specular;

	color = vec4(result,1.0);
};