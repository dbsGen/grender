R"(

attribute vec2 A_TEX_COORD;    //顶点纹理坐标
attribute vec3 A_POSITION;  //顶点位置

varying vec2 TEX_COORD;

uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;

uniform vec3 LIGHT_POSITION;

//用于传递给片元着色器的变量
attribute vec3 A_NORMAL;    //顶点法向量
varying vec4 AMBIENT;
varying vec4 DIFFUSE;
varying vec4 SPECULAR;
//定位光光照计算的方法

void pointLight(                    //定位光光照计算的方法
  in vec3 normal,               //法向量
  inout vec4 ambient,           //环境光最终强度
  inout vec4 diffuse,               //散射光最终强度
  inout vec4 specular,          //镜面光最终强度
  in vec3 lightLocation,            //光源位置
  in vec4 lightAmbient,         //环境光强度
  in vec4 lightDiffuse,         //散射光强度
  in vec4 lightSpecular         //镜面光强度
){
  ambient=lightAmbient;         //直接得出环境光的最终强度
  vec3 normalTarget=A_POSITION+normal;   //计算变换后的法向量
  vec3 newNormal=(MODEL*vec4(normalTarget,1.0)).xyz-(MODEL*vec4(A_POSITION,1.0)).xyz;
  newNormal=normalize(newNormal);   //对法向量规格化
  //计算从表面点到摄像机的向量

  vec3 cameraPos = vec3(MODEL[3]) * -1.0;
  vec3 eye= normalize(cameraPos-(MODEL*vec4(A_POSITION,1)).xyz);
  //计算从表面点到光源位置的向量vp
  vec3 vp= normalize(lightLocation-(MODEL*vec4(A_POSITION,1)).xyz);
  vp=normalize(vp);//格式化vp
  vec3 halfVector=normalize(vp+eye);    //求视线与光线的半向量
  float shininess=50.0;             //粗糙度，越小越光滑
  float nDotViewPosition=max(0.0,dot(newNormal,vp));    //求法向量与vp的点积与0的最大值
  diffuse=lightDiffuse*nDotViewPosition;                //计算散射光的最终强度
  float nDotViewHalfVector=dot(newNormal,halfVector);   //法线与半向量的点积
  float powerFactor=max(0.0,pow(nDotViewHalfVector,shininess));     //镜面反射光强度因子
  specular=lightSpecular*powerFactor;               //计算镜面光的最终强度
}


void main()
{
    mat4 model_view = VIEW * MODEL;
    gl_Position = PROJECTION * model_view * vec4(A_POSITION, 1.0);

    vec4 ambientTemp, diffuseTemp, specularTemp;   //存放环境光、散射光、镜面反射光的临时变量
    pointLight(normalize(A_NORMAL),ambientTemp,diffuseTemp,specularTemp,LIGHT_POSITION,vec4(0.15,0.15,0.15,1.0),vec4(0.9,0.9,0.9,1.0),vec4(0.4,0.4,0.4,1.0));

    AMBIENT=ambientTemp;
    DIFFUSE=diffuseTemp;
    SPECULAR=specularTemp;
    TEX_COORD = A_TEX_COORD;
}
)"