R"(
varying lowp vec4 COLOR;
varying lowp vec2 TEX_COORD;
uniform sampler2D TEXTURE;

void main(void) {
    gl_FragColor = texture2D(TEXTURE, TEX_COORD) * COLOR;
}
)"
