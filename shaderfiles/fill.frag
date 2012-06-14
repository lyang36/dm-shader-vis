//this shader fill the pixels that has zero values with a value specified on other place of the map
uniform vec2 sppos;     //a specified position for reading value to fill the zero 
                        //this is the first step to find the min and max
uniform sampler2D tex;

void main(){
    vec2 xy = vec2(gl_TexCoord[0].st);
    vec4 color = texture2D(tex,xy);
    if(color.r == 0.0){
        color = texture2D(tex,sppos);
    }
    gl_FragColor = color;
}