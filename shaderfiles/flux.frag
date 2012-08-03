//draw projected gaussian profile
#version 150 compatibility
#define PI 3.1415926535897932
uniform vec3 geofac;
            //orthsize, windowsize, pointsize

in vec4 particle;
uniform sampler2D normmap;
uniform int usenormmap;    //whether use the norm map? true: 1 else:0
in vec2 texCoord;
out vec4 gl_FragColor;
in vec4 color;


//This is very important, must be checked 
float profile(vec3 r1,float dtheta){ 
    vec3 r0 = vec3(particle.gba);
    float costheta = dot(r0, r1)/(length(r0)*length(r1));
    //use tylor seriers
    //acos has too much error
    costheta = clamp(costheta, -1.0, 1.0);
    float t2 = 2.0 * ( 1.0 - costheta) + 1.0/3.0*(costheta - 1.0)*(costheta - 1.0) - 4.0/45.0 * (costheta - 1.0) *(costheta - 1.0)*(costheta - 1.0);
    //costheta = clamp(costheta, -1.0, 1.0);
    //float t2 = acos(costheta);
    //t2 = t2*t2;
    float d2 = clamp(t2 / dtheta / dtheta, 0.0, 1.0);
    return exp(- 1.5 * d2);         //here comes the problems
    //return 1.0 - 1.5 * d2;
    
}

//reverse stereoprojection
vec3 prev(vec2 xy){
    float r2 = xy.x*xy.x + xy.y*xy.y;
    return vec3(2.0 * xy.x/(1.0 + r2), 2.0 * xy.y/(1.0 + r2), (r2 - 1.0)/(r2 + 1.0));
}

float projprofile(vec2 xy, float fc, float dtheta){
    return fc * profile(prev(xy), dtheta);
}



void main(){
    //float dsize = particle.r;
    float newsize = particle.r;
	if(newsize == 0.0) discard;
    vec2 xyc = color.rg;
    vec2 coor = xyc *geofac.y / 2.0;
    float flux = 0.0;
    float fluxfac = color.b;
    if(fluxfac != 0.0){
        float dtheta = color.a;
        vec2 p = floor(newsize * texCoord);
        
        p = (p+0.5) / newsize;
        p = 2.0*(p-0.5);
        float u = dot(p, p);
        if (u > 1.0) discard;
        
        vec2 xyp = p * (newsize / 2.0) + coor;
        vec2 xyr = xyp / (geofac.y / 2.0);
        float pr2 = dot(xyr, xyr);
        flux = fluxfac * 4.0/(1.0+pr2)/(1.0+pr2) * profile(prev(xyr), dtheta);
        if(usenormmap == 1){
            float r0 = sqrt(pr2);
            float r = newsize / geofac.z;
            float norm = (texture2D(normmap, vec2(r0, r))).r;
            //if(norm < 0.0) norm = 1.0;
            flux = flux / norm;
        }
        

        gl_FragColor = vec4(flux, 0, 0, 1.0);
    }else{
        gl_FragColor = vec4(0, 0, 0, 0);
    }
    //vec2 p = vec2(gl_TexCoord[0].s,gl_TexCoord[0].t);
    //vec2 p = (texCoord - vec2(0.5, 0.5))*2.0;
    gl_FragColor = vec4(fluxfac, 0.0, 0.0, 1.0);
}
