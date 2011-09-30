uniform sampler2D image;
uniform sampler2D overlay;
uniform vec2 stretch = vec2(1.f, 1.f);
uniform float amount = 0.5f;

void main(){
	gl_FragColor = texture2D(image, gl_TexCoord[0].xy) * (1.f - amount) + texture2D(overlay, fract(gl_TexCoord[0].xy * stretch)) * amount;
}