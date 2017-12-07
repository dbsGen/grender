R"(
varying lowp vec4 COLOR;
varying lowp vec2 TEX_COORD;
varying float SIZE_WIDTH;
varying float SIZE_HEIGHT;
varying float BORDER_WIDTH;
varying float CORNER;

uniform sampler2D TEXTURE;

void main(void) {
    float pixel_width = 1.0/SIZE_WIDTH;
    float pixel_height = 1.0/SIZE_HEIGHT;
    float corner_w = pixel_width * CORNER;
    float corner_h = pixel_height * CORNER;
    float border_w = pixel_width * BORDER_WIDTH;
    float border_h = pixel_height * BORDER_WIDTH;
    
    float p = BORDER_WIDTH/CORNER;
    
    gl_FragColor = texture2D(TEXTURE, TEX_COORD) * COLOR;
    if (TEX_COORD.x >= corner_w && TEX_COORD.x <= 1.0 - corner_w || TEX_COORD.y >= corner_h && TEX_COORD.y <= 1.0 - corner_h)
        gl_FragColor.a *= smoothstep(0.0, border_w, TEX_COORD.x) * smoothstep(1.0, 1.0-border_w, TEX_COORD.x) * smoothstep(0.0, border_h, TEX_COORD.y) * smoothstep(1.0, 1.0-border_h, TEX_COORD.y);
    else if (TEX_COORD.x < corner_w && TEX_COORD.y < corner_h) {
        float center_x = TEX_COORD.x - corner_w;
        float center_y = TEX_COORD.y - corner_h;
        gl_FragColor.a *= smoothstep(1.0, 1.0-p, sqrt(center_x * center_x / corner_w / corner_w + center_y * center_y / corner_h / corner_h));
    }else if (TEX_COORD.x > 1.0-corner_w && TEX_COORD.y > 1.0-corner_h) {
        float center_x = TEX_COORD.x - 1.0 + corner_w;
        float center_y = TEX_COORD.y - 1.0 + corner_h;
        gl_FragColor.a *= smoothstep(1.0, 1.0-p, sqrt(center_x * center_x / corner_w / corner_w + center_y * center_y / corner_h / corner_h));
    }else if (TEX_COORD.x > 1.0-corner_w && TEX_COORD.y < corner_h) {
        float center_x = TEX_COORD.x - 1.0 + corner_w;
        float center_y = TEX_COORD.y - corner_h;
        gl_FragColor.a *= smoothstep(1.0, 1.0-p, sqrt(center_x * center_x / corner_w / corner_w + center_y * center_y / corner_h / corner_h));
    }else if (TEX_COORD.x < corner_w && TEX_COORD.y > 1.0-corner_h) {
        float center_x = TEX_COORD.x - corner_w;
        float center_y = TEX_COORD.y - 1.0 + corner_h;
        gl_FragColor.a *= smoothstep(1.0, 1.0-p, sqrt(center_x * center_x / corner_w / corner_w + center_y * center_y / corner_h / corner_h));
    }
    
//    gl_FragColor.a = step(sqrt(pow(corner_w-TEX_COORD.x, 2.0) + pow(corner_h-TEX_COORD.y, 2.0)), corner_w);
}
)"
