
uniform sampler2D image;
uniform float frame = 0;
//uniform vec2 blocks = vec2(1, 1);
uniform vec2 blocksize = vec2(1, 1);

void main(){
	vec2 blocks = textureSize(image, 0) / blocksize;
	gl_FragColor = texture2D(image, floor(gl_TexCoord[0].xy * blocks) / blocks);
}
