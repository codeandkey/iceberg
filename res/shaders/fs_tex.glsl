#version 130

varying vec2 fs_texcoord;
uniform sampler2D ib_texture;
uniform vec4 ib_color;

void main(void) {
    gl_FragColor = texture2D(ib_texture, fs_texcoord) * ib_color;
}
