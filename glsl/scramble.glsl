//#extension GL_EXT_gpu_shader4: enable

uniform sampler2D image;
uniform float frame;

float generate_random(in vec2 n){
    return fract(sin(dot(n.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
	float randx = generate_random(gl_TexCoord[0].xy * frame);
	float randy = generate_random(gl_TexCoord[0].xy * randx);
	
	gl_FragColor = texture2D(image, vec2(randx, randy));
	
}
	
