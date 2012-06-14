//use to generate a minmax map after few times running
//this must be used in a square map
uniform vec3 params; //{mapsize, blocksize, scales}
                     //mapsize is the full map size in pixels
                     //block size is how many pixels in a block
                     //scales is how many blocks to be counted in a run
                     //initially the blocksize is 1
                     //then 1*scales then 1*scales*scales utill it>mapsize
                     //so mapsize == scales^n
void main(){
    //find the block coordinates -- coordinates in blocksize
    float mapsize = params.x;
    float blocksize = params.y;
    float scales = params.z;
    float fluxmin = 1.0e36;
    float fluxmax = 0.0;
    vec2 nxy = vec2(gl_TexCoord[0].st);
    vec2 xy = floor(nxy * mapsize);
    vec4 color;// = texture2D(tex,nxy);
    vec2 blockxy = floor(nxy / blocksize);
    float supersize = blocksize * scales;
    vec2 superxy = floor(nxy / supersize);      //the coordinates on super-block
    vec2 loopstartblockxy = superxy * scales;   //the start coordinates on block coordinates
    //loop on the super block to find the min and max
    float dx, dy;
    for(dx = loopstartblockxy.x; dx < loopstartblockxy.x + scales; dx ++){
        for (dy = loopstartblockxy.y; dy < loopstartblockxy.y + scales; dy++){
            vec2 pixelxy = vec2(dx, dy) * blocksize / mapsize;
            color = texture2D(tex,pixelxy); 
            if(blocksize == 1){//the first runing
                fluxmax = max(fluxmax, color.r);
            }else{
                fluxmax = max(fluxmax, color.g);
            }
            fluxmin = min(fluxmin, color.r);
        }
    }
    gl_FragColor= vec4(fluxmin, fluxmax, 0, 1);
    //set the color
    
}