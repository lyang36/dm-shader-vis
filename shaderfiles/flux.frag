//draw projected gaussian profile

#define PI 3.1415926535897932
uniform vec3 geofac;
            //orthsize, windowsize, pointsize

varying vec4 particle;
uniform sampler2D normmap;

float profile(vec3 r1,float dtheta){ 
    vec3 r0 = vec3(particle.gba);
    float costheta = dot(normalize(r0), normalize(r1));
    //use tylor seriers
    
    //float t2 = 2.0 * ( 1.0 - costheta);// + 1.0/3.0*(costheta - 1.0)*(costheta - 1.0) - 4.0/45.0 * (costheta - 1.0) *(costheta - 1.0)*(costheta - 1.0);
    costheta = clamp(costheta, -1.0, 1.0);
    float t2 = acos(costheta);
    t2 = t2*t2;
    float d2 = t2 / dtheta / dtheta;
    return costheta;//exp(- 1.5 * d2);
     
    //try flat distribution
    //return 1.0;
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
    vec2 xyc = gl_Color.rg;
    vec2 coor = xyc *geofac.y / 2.0;
    float flux = 0.0;
    float fluxfac = gl_Color.b;
    if(fluxfac != 0.0){
        float dtheta = gl_Color.a;
        vec2 p = floor(newsize * vec2(gl_TexCoord[0].s,gl_TexCoord[0].t));
        
        p = (p+0.5) / newsize;
        p = 2.0*(p-0.5);
        float u = dot(p, p);
        if (u > 1.0) discard;
        
        vec2 xyp = p * (newsize / 2.0) + coor;
        vec2 xyr = xyp / (geofac.y / 2.0);
        float pr2 = dot(xyr, xyr);
        flux = fluxfac * 4.0/(1.0+pr2)/(1.0+pr2) * profile(prev(xyr), dtheta);
        //flux = fluxfac;
        
        float r0 = sqrt(pr2);
        float r = newsize / geofac.y;
        float norm = (texture2D(normmap, vec2(r, r0))).r;
        //flux /= norm;
        

        gl_FragColor = vec4(flux, 0, 0, 1.0);
    }else{
        gl_FragColor = vec4(0, 0, 0, 0);
    }
}
