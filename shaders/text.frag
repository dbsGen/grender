R"(
varying lowp vec4 COLOR;
varying lowp vec2 TEX_COORD;
uniform sampler2D TEXTURE;
uniform float ALPHA;

void main(void) {
    gl_FragColor = COLOR;
    gl_FragColor.a = mix(0.0, COLOR.a, texture2D(TEXTURE, TEX_COORD).a) * ALPHA;
//    gl_FragColor = vec4(1.0,0.0,0.0,1.0);//mix(, COLOR, texture2D(TEXTURE, TEX_COORD).a);
}
)"
