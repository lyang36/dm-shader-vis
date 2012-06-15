//use to generate a minmax map after few times running
//this must be used in a square map
uniform vec3 params; //{mapsize, blocksize, scales}
                     //mapsize is the full map size in pixels
                     //block size is how many pixels in a block
                     //scales is how many blocks to be counted in a run
                     //initially the blocksize is 1
                     //then 1*scales then 1*scales*scales utill it>mapsize
                     //so mapsize == scales^n

uniform sampler2D tex;
void main(){
    //find the block coordinates -- coordinates in blocksize
    float mapsize = params.x;
    float blocksize = params.y;
    float scales = params.z;
    float fluxmin = 1.0e36;
    float fluxmax = 0.0;
    vec2 nxy = vec2(gl_TexCoord[0].st);         //normalized xy
    vec2 xy = floor(nxy * mapsize);             //xy in pixels
    vec4 color;// = texture2D(tex,nxy);
    if(mapsize == blocksize){
        float step = mapsize / scales;
        float dx, dy;
        for(dx = 0.0; dx < mapsize; dx += step){
            for(dy = 0.0; dy < mapsize; dy += step){
                vec2 pixelxy = vec2(dx, dy) / mapsize + 0.1;
                color = texture2D(tex,pixelxy); 
                fluxmin = min(fluxmin, color.r);
                fluxmax = max(fluxmax, color.g);
            }
        }
        
    }else{
        vec2 blockxy = floor(xy / blocksize);       //super size in pixels
        float supersize = blocksize * scales;
        vec2 superxy = floor(xy / supersize);      //the coordinates on super-block
        vec2 loopstartblockxy = superxy * scales;   //the start coordinates on block coordinates
    
    
        //loop on the super block to find the min and max
        float dx, dy;
        for(dx = loopstartblockxy.x; dx < loopstartblockxy.x + scales; dx ++){
            for (dy = loopstartblockxy.y; dy < loopstartblockxy.y + scales; dy++){
                vec2 pixelxy = vec2(dx, dy) * blocksize / mapsize;
                color = texture2D(tex,pixelxy); 
                if(blocksize == 1.0){//the first runing
                    fluxmax = max(fluxmax, color.r);
                }else{
                    fluxmax = max(fluxmax, color.g);
                }
                fluxmin = min(fluxmin, color.r);
            }
        }
    }
    
    gl_FragColor= vec4(fluxmin, fluxmax, 0, 1);
    //gl_FragColor= vec4(0, 1, 0, 1);
    //gl_FragColor = color;
    //gl_FragColor = texture2D(tex,vec2(gl_TexCoord[0].st)); 
    //set the color
    
}