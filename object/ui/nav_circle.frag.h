R"(
varying lowp vec4 COLOR;
varying lowp vec2 TEX_COORD;
uniform float SIZE_WIDTH;
uniform float SIZE_HEIGHT;
uniform float BORDER_WIDTH;
uniform vec4 BORDER_COLOR;
uniform float CORNER;

void main(void) {
    float border = BORDER_WIDTH / SIZE_WIDTH;
    float dx = 0.5 - TEX_COORD.x;
    float dy = 0.5 - TEX_COORD.y;
    float off = sqrt(dx * dx + dy * dy);
    float bs = 0.5 - border;
    float p = smoothstep(bs, bs + 0.02, off);
    gl_FragColor = COLOR * (1.0-p) + BORDER_COLOR * p;
    gl_FragColor.a = smoothstep(0.5, 0.48, off) * COLOR.a;
}
)"
