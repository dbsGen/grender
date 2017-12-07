R"(
varying lowp vec4 COLOR;
varying lowp vec2 TEX_COORD;
uniform sampler2D TEXTURE;
uniform float SIZE_WIDTH;
uniform float SIZE_HEIGHT;
uniform float BORDER_WIDTH;
uniform vec4 BORDER_COLOR;
uniform float CORNER;
uniform float ALPHA;

void main(void) {
    float pixel_width = 1.0/SIZE_WIDTH;
    float pixel_height = 1.0/SIZE_HEIGHT;
    float corner_w = pixel_width * CORNER;
    float corner_h = pixel_height * CORNER;
    float border_w = pixel_width * BORDER_WIDTH;
    float border_h = pixel_height * BORDER_WIDTH;
    float weight = 0.0;
    float border_p = 0.0;
    if (CORNER > 0.0) {
        border_p = border_w / corner_w;

        if (TEX_COORD.x < corner_w && TEX_COORD.y < corner_h) {
            float center_x = TEX_COORD.x - corner_w;
            float center_y = TEX_COORD.y - corner_h;
            weight = sqrt(center_x * center_x / corner_w / corner_w + center_y * center_y / corner_h / corner_h);
        }else if (TEX_COORD.x > 1.0-corner_w && TEX_COORD.y > 1.0-corner_h) {
            float center_x = TEX_COORD.x - 1.0 + corner_w;
            float center_y = TEX_COORD.y - 1.0 + corner_h;
            weight = sqrt(center_x * center_x / corner_w / corner_w + center_y * center_y / corner_h / corner_h);
        }else if (TEX_COORD.x > 1.0-corner_w && TEX_COORD.y < corner_h) {
            float center_x = TEX_COORD.x - 1.0 + corner_w;
            float center_y = TEX_COORD.y - corner_h;
            weight = sqrt(center_x * center_x / corner_w / corner_w + center_y * center_y / corner_h / corner_h);
        }else if (TEX_COORD.x < corner_w && TEX_COORD.y > 1.0-corner_h) {
            float center_x = TEX_COORD.x - corner_w;
            float center_y = TEX_COORD.y - 1.0 + corner_h;
            weight = sqrt(center_x * center_x / corner_w / corner_w + center_y * center_y / corner_h / corner_h);
        }else {
            weight = max(max(1.0-TEX_COORD.x / corner_w, 1.0-(1.0-TEX_COORD.x) / corner_w), max(1.0-TEX_COORD.y / corner_h, 1.0-(1.0-TEX_COORD.y) / corner_h));
        }
        gl_FragColor = mix(BORDER_COLOR, texture2D(TEXTURE, TEX_COORD) * COLOR, step(weight, 1.0-border_p));
        gl_FragColor.a = mix(0.0, gl_FragColor.a, smoothstep(0.0,1.0/CORNER,1.0-weight));
    } else if (BORDER_WIDTH == 0.0) {
        gl_FragColor = texture2D(TEXTURE, TEX_COORD) * COLOR;
    } else {
        weight = min(min((1.0-TEX_COORD.x)/border_w, TEX_COORD.x/border_w),
                     min((1.0-TEX_COORD.y)/border_h, TEX_COORD.y/border_h));
        gl_FragColor = mix(texture2D(TEXTURE, TEX_COORD) * COLOR, BORDER_COLOR, step(weight, 1.0));
    }
    gl_FragColor.a = gl_FragColor.a * ALPHA;
}
)"

