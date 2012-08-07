void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	vec4 v = vec4(gl_Vertex);
    gl_FrontColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * v;	
} 
