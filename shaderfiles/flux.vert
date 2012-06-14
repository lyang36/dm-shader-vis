//particle shader
//convert the particles to point sprite
/*input postion and parameter of exactly a particle*/
/*outout: xc, yc, r, dtheta, */


//the new coordinate system based on the vpos
#define PI 3.1415926535897932

uniform vec3 zaxis;     //vpos
uniform vec3 xaxis;
uniform vec3 yaxis;
uniform vec3 opos;
uniform vec3 geofac;
                //geometry fact{size(square),viewportsize, maxpointsize }
//uniform float sdthetha;

varying vec4 particle;    //the radius of the particle circle and the coordianate

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
    
    vec3 npvec = normalize(pvec);

    vec3 nzaxis = normalize(zaxis);
    vec3 nxaxis = normalize(xaxis);
    vec3 nyaxis = normalize(yaxis);
    
    
    float costheta = dot(npvec, nzaxis);
    //costheta -3.0 / 5.0;
    float theta = acos(costheta);      //0.955
    
    vec3 newpvec = vec3(dot(npvec, nxaxis), dot(npvec, nyaxis),costheta);
    
    if((theta > PI / 2.0 || theta + dtheta >= PI / 2.0) && dtheta < PI / 2.0){
        float sintheta = sin(theta);
        float sinphi;
        float cosphi;
        if(sintheta < 1.0e-8 ){
            //phi = 0.0;
            sinphi = 0.0;
            cosphi = 1.0;
        }else{
            sinphi = newpvec.y / sintheta;
            cosphi = newpvec.x / sintheta;
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
        float newsize = r *geofac.y;

        
        
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
    
    
        //calculate the flux infomation
        float c = 1.5;
        float d2 = dtheta * dtheta;
        float normfac;
        if(newsize == 1.0){ //only one pixel!
            normfac = 1.0;
            dtheta = 0.0;
        }else{
            normfac = c * exp(c) / (exp(c) - 1.0) / PI / d2; //(normalize the gaussian profile)
        }
        //Integrate[2 \[Pi] A Exp[-c \[Theta]^2/\[Theta]0^2] \[Theta], {\[Theta],0, \[Theta]0}] == 1
        
        //Must add another vector (xc, yc)
        gl_FrontColor = vec4(xc, yc, normfac * flux , dtheta);
        particle = vec4(dsize, newpvec.x, newpvec.y, newpvec.z);
    
        //texture
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
