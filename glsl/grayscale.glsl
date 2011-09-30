//#version 400

uniform sampler2D image;
uniform vec3 weight = vec3(0.21f, 0.71f, 0.07f);

void main(){
	gl_FragColor.xyz = vec3(dot(texture2D(image, gl_TexCoord[0].xy).xyz, weight));
	gl_FragColor.w = texture2D(image, gl_TexCoord[0].xy).w;
}
