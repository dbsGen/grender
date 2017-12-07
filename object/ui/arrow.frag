R"(
varying lowp vec4 COLOR;
varying lowp vec2 TEX_COORD;

void main(void) {
    float a = TEX_COORD.x - 2.0 * TEX_COORD.y;
    float b = 2.0 - TEX_COORD.x - 2.0 * TEX_COORD.y;
    
    gl_FragColor = COLOR;
    gl_FragColor.a = clamp(smoothstep(0.2, 0.1, abs(a)) + smoothstep(0.2, 0.1, abs(b)), 0.0, 1.0);
}
)"
