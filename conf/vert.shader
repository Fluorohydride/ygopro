#version 330
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec4 v_hcolor;
layout (location = 3) in vec2 v_texcoord;
uniform mat4 mvp;
out vec4 color;
out vec4 hcolor;
out vec2 texcoord;

void main() {
  color = v_color;
  hcolor = v_hcolor;
  texcoord = v_texcoord;
  gl_Position = mvp * vec4(v_position, 1.0);
}
