//particle shader
//convert the particles to point sprite
/*input postion and parameter of exactly a particle*/
/*outout: xc, yc, r, dtheta, */

//GEOMETRY SHADER
#version 330
#extension GL_EXT_geometry_shader4 : enable
//the new coordinate system based on the vpos
#define PI 3.1415926535897932

uniform mat3 rotmatrix; //rotation matrix
uniform vec3 opos;
uniform vec3 geofac;
                //geometry fact{size(square),viewportsize, maxpointsize }
//uniform float sdthetha;

inout vec4 particle;    //the radius of the particle circle and the coordianate
        //size, x, y, z

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;
//layout (points) out;

in VertexData {
    vec2 texCoord;
    vec3 normal;
} VertexIn;
 
out VertexData {
    vec2 texCoord;
    vec3 normal;
} VertexOut;

uniform int usenormmap;    //whether use the norm map? true: 1 else:0

float sc = 100.0 ;
 
void main(){
  
     // copy attributes
	gl_Position.x = 1.0; 
	gl_Position.y = -1.0;
	gl_Position.z = 0.5;
	gl_Position.w = 1.0;
	 
    //gl_Position = //gl_in[0].gl_Position;
	//gl_Position.x = gl_Position.x/gl_Position.w - sc * particle.r / 2.0;
	//gl_Position.y = gl_Position.y/gl_Position.w - sc ;//;//* particle.r / 2.0;
    VertexOut.normal = VertexIn.normal;
    VertexOut.texCoord = VertexIn.texCoord;
    // done with the vertex
    EmitVertex();

	gl_Position.x = -1.0; 
	gl_Position.y = -1.0;
	EmitVertex();

	gl_Position.x = 0.0; 
	gl_Position.y = 1.0;
	EmitVertex();	
	
	
 /*    // copy attributes
    gl_Position = gl_in[0].gl_Position;
	gl_Position.x = gl_Position.x/gl_Position.w + sc ;//* particle.r / 2.0;
	gl_Position.y = gl_Position.y/gl_Position.w - sc ;//* particle.r / 2.0;
    VertexOut.normal = VertexIn.normal;
    VertexOut.texCoord = VertexIn.texCoord;
    // done with the vertex
    EmitVertex();

     // copy attributes
    gl_Position = gl_in[0].gl_Position;
	gl_Position.x = gl_Position.x/gl_Position.w - sc ;//* particle.r / 2.0;
	gl_Position.y = gl_Position.y/gl_Position.w + sc ;//* particle.r / 2.0;
    VertexOut.normal = VertexIn.normal;
    VertexOut.texCoord = VertexIn.texCoord;
    // done with the vertex
    EmitVertex();

     // copy attributes
    gl_Position = gl_in[0].gl_Position;
	gl_Position.x = gl_Position.x/gl_Position.w + sc ;//* particle.r / 2.0;
	gl_Position.y = gl_Position.y/gl_Position.w + sc ;//* particle.r / 2.0;
    VertexOut.normal = VertexIn.normal;
    VertexOut.texCoord = VertexIn.texCoord;
    // done with the vertex
    EmitVertex();	
	*/
  EndPrimitive();
}