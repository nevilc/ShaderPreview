uniform sampler2D image;
uniform float threshold = 0.7f; // [0.0 - 1.0]
uniform float magnitude = 0.9f; // [0.0 - 1.0] lower is more intense!

void main(){
	vec4 p = texture2D(image, gl_TexCoord[0].xy);
	p -= threshold;
	p = vec4(sign(p.x) * pow(abs(p.x), magnitude), sign(p.y) * pow(abs(p.y), magnitude), sign(p.z) * pow(abs(p.z), magnitude), p.w);
	p += threshold;
	
	gl_FragColor = p;
	
}

