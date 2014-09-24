#version 330
in vec4 color;
in vec4 hcolor;
in vec2 texcoord;
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
