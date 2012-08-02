//particle shader
//convert the particles to point sprite
/*input postion and parameter of exactly a particle*/
/*outout: xc, yc, r, dtheta, */

//GEOMETRY SHADER
#version 150
//the new coordinate system based on the vpos
#define PI 3.1415926535897932

uniform mat3 rotmatrix; //rotation matrix
uniform vec3 opos;
uniform vec3 geofac;
                //geometry fact{size(square),viewportsize, maxpointsize }
//uniform float sdthetha;

varying vec4 particle;    //the radius of the particle circle and the coordianate
        //size, x, y, z

uniform int usenormmap;    //whether use the norm map? true: 1 else:0
 
void main(){
    
}