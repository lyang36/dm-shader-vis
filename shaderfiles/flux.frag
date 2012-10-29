//draw projected gaussian profile

#define PI 3.1415926535897932
uniform vec3 geofac;
            //orthsize, windowsize, pointsize

varying vec4 particle;
uniform sampler2D normmap;
uniform int usenormmap;    //whether use the norm map? true: 1 else:0
//float lim_r = 0.5;         //the limite of the r observed, beyond which the fragment will be discarded.

varying float viewangle_scale_factor; //the same with scale factor


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
    float d2 = (t2 / dtheta / dtheta, 0.0, 1.0);
    if(t2 > 1.0){
        return 0.0;
    }
    if(t2 < 0.0){
        t2 = 0.0;
    }
    return exp(- 1.5 * d2); 
}

//reverse stereoprojection
vec3 prev(vec2 xy){
    xy = xy / viewangle_scale_factor;
    float r2 = xy.x*xy.x + xy.y*xy.y;
    return vec3(2.0 * xy.x/(1.0 + r2), 2.0 * xy.y/(1.0 + r2), (r2 - 1.0)/(r2 + 1.0));
}


void main(){
    float newsize = particle.r;
    vec2 xyc = gl_Color.rg;
    vec2 coor = xyc *geofac.y / 2.0;
    float flux = 0.0;
    float fluxfac = gl_Color.b;

	float dtheta = gl_Color.a;
	vec2 p = floor(newsize * vec2(gl_TexCoord[0].s,gl_TexCoord[0].t));
			
	p = (p+0.5) / newsize;
	p = 2.0*(p-0.5);
	float u = dot(p, p);
	if (u > 1.0) discard;
			
	vec2 xyp = p * (newsize / 2.0) + coor;
	vec2 xyr = xyp / (geofac.y / 2.0);
	float pr2 = dot(xyr, xyr);
	
	//seting up the view angle
    //float lim_r = 1.0 / viewangle_scale_factor;
	if(pr2 > 1.0){
        discard;
	}

	if(newsize != 1.0){
		if(fluxfac != 0.0){
			//flux = fluxfac;
			if(usenormmap == 1){
				float r0 = sqrt(pr2);
				float r = newsize / geofac.z;
				float norm = (texture2D(normmap, vec2(r0, r))).r;
				//if(norm < 0.0) norm = 1.0;
				flux = flux / norm;
			}else{
                //be careful to the units
                //flux = fluxfac * 4.0/(1.0+pr2)/(1.0+pr2) * profile(prev(xyr), dtheta);
                //
                flux = fluxfac * profile(prev(xyr), dtheta) *  4.0/(1.0+pr2)/(1.0+pr2);
            }
			

			gl_FragColor = vec4(flux, 0, 0, 1.0);
		}else{
			discard;
		}
	}else{
		gl_FragColor = vec4(fluxfac, 0, 0, 1.0);
	}
}
