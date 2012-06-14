#define MAX 6.0e6
#define MIN 1.0e4
uniform sampler2D tex;

void main()
{
    vec2 xy = vec2(gl_TexCoord[0].st);
    vec4 color = texture2D(tex,xy);
    float flux = color.r;
    if(flux == 0.0){
        flux = MIN;
    }
    //flux = .1;
    float ff;
    ff= clamp((log(flux) - log(MIN))/(log(MAX)-log(MIN)), 0.0 , 1.0);
    //ff = clamp((flux - MIN)/(MAX - MIN), 0.0 , 1.0);
    //color = vec4(ff, 0, 0.5, 1);
    
    if(dot(xy-0.5,xy-0.5) <= 0.25){
        color = vec4(ff, (ff*(1.0 - ff) * 4.0), (1.0 - ff),1);
    }else{
        color = vec4(0.0, 0.0, 0.0, 0.0);
    }
	gl_FragColor = color;
}
