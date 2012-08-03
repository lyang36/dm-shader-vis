//particle shader
//convert the particles to point sprite
/*input postion and parameter of exactly a particle*/
/*outout: xc, yc, r, dtheta, */
//TODO: rewrite the normalization function
#version 150
//the new coordinate system based on the vpos
#define PI 3.1415926535897932


//uniform float sdthetha;
//varying float geo_newsize;
in vec4 gl_Color;
in vec4  gl_Vertex;        
out vec4 gl_FrontColor;  
out block
{
	vec4 gl_Color;
	vec4 particle; 
} bl_out;

void main(){
    bl_out.gl_Color = gl_Color;
	bl_out.particle = gl_Vertex;
}
