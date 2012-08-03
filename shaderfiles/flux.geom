//particle shader
//convert the particles to point sprite
/*input postion and parameter of exactly a particle*/
/*outout: xc, yc, r, dtheta, */

//GEOMETRY SHADER
#version 150
//the new coordinate system based on the vpos
#define PI 3.1415926535897932

layout(points) in;
layout(triangle_strip, max_vertices=4) out;
//layout(points) out;
out vec2 texCoord;
out vec4 color;
out vec4 particle;
in block
{
	vec4 gl_Color;
	vec4 particle; 
} bl_In[];

//in float abc;

 void main()
{
	//QUESTION: how the coordinates works
    vec4 point;

	int i = 0;
	color = bl_In[i].gl_Color;
	particle = bl_In[i].particle;
    float psize = gl_in[i].gl_PointSize;
	//if(psize == 0.0) discard;
    // copy attributes
	
	if(psize != 0.0)
	{
		point = vec4(gl_in[i].gl_Position.x, gl_in[i].gl_Position.y, 0.0, 1.0);
		texCoord = vec2(0,0);
		gl_Position = point + vec4(-psize, -psize, 0, 0);
		EmitVertex();
		texCoord = vec2(0,1);
		gl_Position = point + vec4(-psize, psize, 0, 0);
		EmitVertex();
		texCoord = vec2(1,0);
		gl_Position = point + vec4(psize, -psize, 0, 0);
		EmitVertex();
		texCoord = vec2(1,1);
		gl_Position = point + vec4(psize, psize, 0, 0);
		EmitVertex();
		//EndPrimitive();
	}
}