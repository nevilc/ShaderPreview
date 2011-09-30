require 'glsl'

uniforms = {
	frame = 0,
	timer = 0
}

function main(delta)
	uniforms.frame = uniforms.frame + 1
	uniforms.timer = uniforms.timer + delta
end
