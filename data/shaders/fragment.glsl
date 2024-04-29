#version 460 core
in vec2 f_texture_coords;

out vec4 FragColor;

uniform sampler2D u_texture;

void main() {
  FragColor = texture(u_texture, f_texture_coords);
}
