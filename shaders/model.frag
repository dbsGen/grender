R"(
uniform sampler2D TEXTURE;//纹理内容数据
//接收从顶点着色器过来的参数
varying vec4 AMBIENT;
varying vec4 DIFFUSE;
varying vec4 SPECULAR;
varying vec2 TEX_COORD;

void main()
{
    //将计算出的颜色给此片元
    vec4 finalColor=texture2D(TEXTURE, TEX_COORD);
    //给此片元颜色值
    gl_FragColor = finalColor * clamp((AMBIENT + DIFFUSE + SPECULAR), vec4(0.0), vec4(1.0));
}
)"
