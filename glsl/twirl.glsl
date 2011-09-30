
uniform sampler2D image;

uniform float rotation = 0.0f;
uniform vec2 center = vec2(0.5f, 0.5f);

const float pi=3.1415926535f;

void main(){
	vec2 normalized = gl_TexCoord[0].xy - center;
	float direction = atan(normalized.y, normalized.x);
	float magnitude = sqrt(pow(normalized.x, 2) + pow(normalized.y, 2));
	direction += rotation * (magnitude);
	normalized = vec2(magnitude * cos(direction), magnitude * sin(direction));
	normalized += center;
	gl_FragColor = texture2D(image, normalized);
}
