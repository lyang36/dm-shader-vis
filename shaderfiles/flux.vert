//particle shader
//convert the particles to point sprite
/*input postion and parameter of exactly a particle*/
/*outout: xc, yc, r, dtheta, */


//the new coordinate system based on the vpos
#define PI 3.1415926535897932

uniform mat3 rotmatrix; //rotation matrix
uniform vec3 opos;
uniform vec3 geofac;
                //geometry fact{size(square),viewportsize, maxpointsize }
//uniform float sdthetha;

varying vec4 particle;    //the radius of the particle circle and the coordianate
        //size, x, y, z
            
float profile(vec3 r1, float dtheta){ 
    vec3 r0 = vec3(particle.gba);
    float costheta = dot(normalize(r0), normalize(r1));
    //use tylor seriers
    //float t2 = 2.0 * ( 1.0 - costheta);// + 1.0/3.0*(costheta - 1.0)*(costheta - 1.0) - 4.0/45.0 * (costheta - 1.0) *(costheta - 1.0)*(costheta - 1.0);
    costheta = clamp(costheta, -1.0, 1.0);
    float t2 = acos(costheta);
    t2 = t2*t2;
    float d2 = t2 / dtheta / dtheta;
    return costheta;//exp(- 1.5 * d2);
}

//reverse stereoprojection
vec3 prev(vec2 xy){
    float r2 = xy.x*xy.x + xy.y*xy.y;
    return vec3(2.0 * xy.x/(1.0 + r2), 2.0 * xy.y/(1.0 + r2), (r2 - 1.0)/(r2 + 1.0));
}


float calc_norm(vec2 svec, float newsize, float dtheta){
    float norm = 0.0;
    
    vec2 coor = svec * geofac.y / 2.0;
    
    
    float x=0.0;
    float y=0.0;
    for(x = 0.0; x < newsize; x++){
        for(y = 0.0; y < newsize; y++){
            float px = (x+0.5)/newsize;
            float py = (y+0.5)/newsize;
            px = 2.0*(px-0.5); // -1...1
            py = 2.0*(py-0.5);
            vec2 xy = vec2(px, py);
            float u = dot(xy, xy);
            if (u > 1.0)
                continue;
            
            vec2 xyp = xy * (newsize / 2.0) + coor;
            vec2 xyr = xyp / (geofac.y / 2.0);
            float pr2 = dot(xyr, xyr);
            norm += 4.0/(1.0+pr2)/(1.0+pr2) * profile(prev(xyr), dtheta);
            //norm += 1.0;
            
        }
    }
    return 1.0/norm;
    //return 1.0 / (newsize * newsize);
}


void main(){
/*    gl_PointSize = 100.0;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = vec4(xaxis.x,yaxis.y, zaxis.z,0);*/
    
    vec4 newpos;
   //find the angle
    vec3 pvec = vec3(gl_Vertex) - opos;    
                                        //input x, y z of the particle
                                        //transform it to the stereoprojection plane
    vec4 parameter = vec4(gl_Color);    //parameters
                                        //mass, density and hsmooth
    
    float dtheta;                       //anular radias
    //float phi;
    float dsize;
 
    
    float distance = length(pvec);
    
    
    dtheta = parameter.b / distance;    //2.186
    
    //rotation and normalize
    vec3 npvec = normalize(rotmatrix * pvec);
    
    
    float costheta = npvec.z;//dot(npvec, nzaxis);
    //costheta -3.0 / 5.0;
    float theta = acos(costheta);      //0.955
    
    //vec3 newpvec = vec3(dot(npvec, nxaxis), dot(npvec, nyaxis),costheta);
    
    if((theta > PI / 2.0 || theta + dtheta >= PI / 2.0) && dtheta < PI / 2.0)
    {
        float sintheta = sin(theta);
        float sinphi;
        float cosphi;
        if(sintheta < 1.0e-8 ){
            //phi = 0.0;
            sinphi = 0.0;
            cosphi = 1.0;
        }else{
            sinphi = npvec.y/sintheta;//newpvec.y / sintheta;
            cosphi = npvec.x/sintheta;//newpvec.x / sintheta;
        }
        
        //phi= PI / 2.0;
        
        float flux = parameter.g * parameter.r / (4.0 * PI * distance * distance);
    
    
        float xc, yc, r;
    
        //transform the vertex:
        //stereoproject a circle
        float sintpr = sin(theta + dtheta);
        float costpr = cos(theta + dtheta);
        float sintmr = sin(theta - dtheta);
        float costmr = cos(theta - dtheta);
        float a = sintpr/(1.0-costpr);
        float b = sintmr/(1.0-costmr);
        r = -(a - b)/2.0;
        float prho = (a + b)/2.0;
        xc = prho * cosphi;
        yc = prho * sinphi;
        float newsize = floor(r *geofac.y); ///!!!!!!!!!!!!!!!!

        
        newpos = vec4(xc * geofac.x, yc * geofac.x, 0.0, 1.0);
        
        if(newsize > geofac.z){
            dsize = geofac.z / newsize * r;
            newsize = geofac.z;
        }else{
            dsize = r;
        }
        
        if(newsize < 1.0){
            newsize = 1.0;
        }
        gl_PointSize = newsize;  //point size
    

        //calculate normfac
        //particle must be written before fhe nomal fac
        //particle = vec4(dsize, npvec.x, npvec.y, npvec.z);
        particle = vec4(newsize, npvec.x, npvec.y, npvec.z);
        
        float normfac;
        float d2 = dtheta * dtheta;
        {
            //normfac = calc_norm(vec2(xc, yc), newsize, dtheta);
            normfac = 1.0;
            
        }
        
        //Must add another vector (xc, yc)
        //flux = 1.0;
        gl_FrontColor = vec4(xc, yc, flux * normfac , dtheta);

        gl_TexCoord[0] = gl_MultiTexCoord0;
    }else{
        gl_PointSize = 1.0;  //point size
        newpos = vec4(0.0, 0.0, 0.0, 1.0);
        gl_FrontColor = vec4(0, 0, 0, 0);
        gl_TexCoord[0] = gl_MultiTexCoord0;
    }
    //newpos = vec4(-3,-3, 0, 1);
    //gl_PointSize = 200.0;  //point size
    gl_Position = gl_ModelViewProjectionMatrix * newpos;    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    //gl_FrontColor = vec4(dtheta,0,0,1);
    //dsize = 0.4;
    
}
