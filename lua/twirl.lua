require 'glsl'

uniform = {
	rotation = 0
	
}

setting = {
	fullscreen = false
}

limit = 5.0
rate = 2.5

function main(delta)
	uniform.rotation = uniform.rotation + rate * delta
	if uniform.rotation > limit or uniform.rotation < -limit then
		rate = -rate
	end
end
