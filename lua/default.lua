require 'glsl'

uniform = {
	frame = 0,
	timer = 0
}

function main(delta)
	uniform.frame = uniform.frame + 1
	uniform.timer = uniform.timer + delta
end
