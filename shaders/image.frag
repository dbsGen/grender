R"(
varying lowp vec4 COLOR;
varying lowp vec2 TEX_COORD;
uniform sampler2D TEXTURE;

uniform float SIZE_WIDTH;
uniform float SIZE_HEIGHT;

void main(void) {
    float pixel_width = 1.0/SIZE_WIDTH;
    float pixel_height = 1.0/SIZE_HEIGHT;

    gl_FragColor = texture2D(TEXTURE, TEX_COORD)*COLOR;
}
)"

