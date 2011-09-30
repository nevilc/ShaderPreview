
uniform sampler2D image;

uniform vec4 edgecolor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform vec4 noncolor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
uniform float alphathreshold = 0.0;
uniform vec4 transcolor = vec4(1.0f, 0.0f, 1.0f, 1.0f);

vec2 pixelsize = 1.0f / textureSize(image, 0);

const vec2 left = 	vec2(-1.0f,  0.0f);
const vec2 right = 	vec2( 1.0f,  0.0f);
const vec2 up = 	vec2( 0.0f, -1.0f);
const vec2 down = 	vec2( 0.0f,  1.0f);


void main(){
	if (texture2D(image, gl_TexCoord[0].xy).w 	 > alphathreshold){
		if (texture2D(image, gl_TexCoord[0].xy + pixelsize * left).w > alphathreshold && 
			texture2D(image, gl_TexCoord[0].xy + pixelsize * right).w > alphathreshold && 
			texture2D(image, gl_TexCoord[0].xy + pixelsize * up).w > alphathreshold && 
			texture2D(image, gl_TexCoord[0].xy + pixelsize * down).w > alphathreshold){
				gl_FragColor = noncolor;
		}else{
			gl_FragColor = edgecolor;
		}
	}else{
		gl_FragColor = transcolor;
	}
}
