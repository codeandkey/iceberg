#version 130

attribute vec2 position;
attribute vec2 texcoord;

varying vec2 fs_texcoord;

/* perform most of the transforms here.
 * not sure if uniform states are faster than just precomputing transforms back on the cpu but we'll try it */

uniform float ib_rot;
uniform vec2 ib_pos;
uniform vec2 ib_scale;
uniform mat4x4 ib_camera;

void main(void) {
    /* scale -> rotate -> translate -> camera -> projection */

    vec2 inter_pos = position;

    /* perform scale transform */
    inter_pos *= ib_scale;

    /* rotate if nonzero angle */
    if (ib_rot != 0.0) {
        float s = sin(ib_rot);
        float c = cos(ib_rot);

        mat2 rm = mat2(c, s, -s, c);
        inter_pos = rm * inter_pos;
    }

    /* translate */
    inter_pos += ib_pos;

    /* bring into 4-space so we can apply matrices */
    gl_Position = vec4(inter_pos, 0.0, 1.0);

    /* apply camera */
    gl_Position = ib_camera * gl_Position;

    /* send texcoords to fragment */
    fs_texcoord = texcoord;
}
