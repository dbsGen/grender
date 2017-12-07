R"(
attribute vec4 A_COLOR;
attribute vec2 A_TEX_COORD;
attribute vec3 A_POSITION;

varying vec2 TEX_COORD;
varying vec4 COLOR;

uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;

void main() {
    COLOR = A_COLOR;
    mat4 model_view = VIEW * MODEL;
    TEX_COORD = A_TEX_COORD;
    gl_Position = PROJECTION * model_view * vec4(A_POSITION, 1.0);
}
)"
