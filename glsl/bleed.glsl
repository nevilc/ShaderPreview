uniform sampler2D image;
uniform float frame;

uniform float radius = 1.f;

int iradius = int(radius);
int samples = 1 + 2*(int(pow(iradius, 2)) + iradius);
vec2 pixelsize = vec2(1.f, 1.f) / textureSize(image, 0);

void main(){
	vec4 newcolor = vec4(0.f, 0.f, 0.f, 0.f);
	
	ivec2 v;
	for (v.x = -iradius; v.x <= iradius; ++v.x){
		for (v.y = abs(v.x) - iradius; v.y <= iradius - abs(v.x); ++v.y){
			newcolor += texture2D(image, (pixelsize * v) + gl_TexCoord[0].xy);
		}
	}
	gl_FragColor = newcolor / samples;
}