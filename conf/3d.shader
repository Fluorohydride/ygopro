//vertex shader
#version 330
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_texcoord;
layout (location = 2) in vec4 v_color;
layout (location = 3) in vec4 v_hcolor;
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

//fragment shader
#version 330
in vec2 texcoord;
in vec4 color;
in vec4 hcolor;
layout (location = 0) out vec4 frag_color;
uniform sampler2D texid;

void main() {
  if(gl_FrontFacing) {
    vec4 texcolor = texture(texid, texcoord);
    frag_color = mix(texcolor * color, vec4(hcolor.r, hcolor.g, hcolor.b, 1.0), hcolor.a);
  } else {
    discard;
  }
}
