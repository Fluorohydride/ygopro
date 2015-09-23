//vertex shader
#version 330
layout (location = 0) in vec2 v_position;
layout (location = 1) in vec2 v_texcoord;
layout (location = 2) in vec4 v_color;
layout (location = 3) in vec4 v_hcolor;
out vec2 texcoord;
out vec4 color;
out vec4 hcolor;

void main() {
  texcoord = v_texcoord;
  color = v_color;
  hcolor = v_hcolor;
  gl_Position = vec4(v_position, 0.0, 1.0);
}

//fragment shader
#version 330
in vec2 texcoord;
in vec4 color;
in vec4 hcolor;
layout (location = 0) out vec4 frag_color;
uniform sampler2D texid;
uniform float alpha;

void main() {
  vec4 texcolor = texture(texid, texcoord);
  frag_color = mix(texcolor * color * vec4(1.0, 1.0, 1.0, alpha), vec4(hcolor.r, hcolor.g, hcolor.b, 1.0), hcolor.a);
}
