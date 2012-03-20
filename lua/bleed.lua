require 'glsl'

uniform = {
	radius = 0
}

limit = 8.0
rate = 4.0

function main(delta)
	uniform.radius = uniform.radius + rate * delta
	if uniform.radius > limit or uniform.radius < 0 then
		rate = -rate
	end
end
