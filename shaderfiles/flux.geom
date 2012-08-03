//particle shader
//convert the particles to point sprite
/*input postion and parameter of exactly a particle*/
/*outout: xc, yc, r, dtheta, */

//GEOMETRY SHADER
#version 150
//#extension GL_EXT_geometry_shader4:enable
//the new coordinate system based on the vpos
#define PI 3.1415926535897932

uniform mat3 rotmatrix; //rotation matrix
uniform vec3 opos;
uniform vec3 geofac;
                //geometry fact{size(square),viewportsize, maxpointsize }
//uniform float sdthetha;

layout(points) in;
layout (triangle_strip, max_vertices=4) out;
//layout(points) out;
out vec2 texCoord;
inout vec4 particle;;

 void main()
{
	//QUESTION: how the coordinates works
    vec4 point;
	int i = 0;
    float psize = gl_in[i].gl_PointSize;
    //float psize = 0.5;

        // copy attributes
    point = gl_in[i].gl_Position;
    //VertexOut.normal = VertexIn[i].normal;
    texCoord = vec2(0,0);
    gl_Position = point + vec4(-psize, -psize, 0, 1);
    EmitVertex();
    texCoord = vec2(1,0);
    gl_Position = point + vec4(psize, -psize, 0, 1);
    EmitVertex();
    texCoord = vec2(0,1);
    gl_Position = point + vec4(-psize, psize, 0, 1);
    EmitVertex();
    texCoord = vec2(1,1);
    gl_Position = point + vec4(psize, psize, 0, 1);
    EmitVertex();
    
    EndPrimitive();
}