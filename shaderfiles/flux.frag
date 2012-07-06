//draw projected gaussian profile

#define PI 3.1415926535897932
uniform vec3 geofac;
            //orthsize, windowsize, pointsize

varying vec4 particle;

float profile(vec3 r1,float dtheta){ 
    vec3 r0 = vec3(particle.gba);
    float costheta = dot(r0, r1);
    //use tylor seriers
    
    float t2 = 2.0 * ( 1.0 - costheta);// + 1.0/3.0*(costheta - 1.0)*(costheta - 1.0) - 4.0/45.0 * (costheta - 1.0) *(costheta - 1.0)*(costheta - 1.0);
    //float t2 = acos(costheta);
    //t2 = t2*t2;
    float d2 = t2 / dtheta / dtheta;
    return exp(- 1.5 * d2);
     
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
    float dsize = particle.r;
    vec2 xyc = gl_Color.rg;
    float flux = 0.0;
    float fluxfac = gl_Color.b;
    if(fluxfac != 0.0){
        float dtheta = gl_Color.a;
        if(dtheta > 0.0){   //pixel == 1
            vec2 xy = (gl_TexCoord[0].st - vec2(0.5, 0.5)) * 2.0;
            vec2 xyr = xy  * dsize;
            xyr = xyr + xyc;           //be careful to the orientation
            
            xyr = floor(xyr * geofac.y/2.0)/geofac.y*2.0;
            
            float rho2 = dot(xyr,xyr);
            //&& dot(xyr, xyr) <= 1.0  && rho2 <= 1.0
            if(dot(xy, xy) <= 1.0){

                float fact = 4.0 / (1.0 + rho2) / (1.0 + rho2);
                //actual distribution
                flux = projprofile(xyr, fluxfac * fact, dtheta);
                //flat distribution
                //flux = gl_Color.b;
                
            }
        }else{
            flux = fluxfac; 
        }
        gl_FragColor = vec4(flux, 0, 0, 1.0);
    }else{
        gl_FragColor = vec4(0, 0, 0, 0);
    }
    //gl_FragColor = gl_Color;
}
