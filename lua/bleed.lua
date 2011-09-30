require 'glsl'

uniforms = {
	radius = 0
}

limit = 8.0
rate = 4.0

function main(delta)
	uniforms.radius = uniforms.radius + rate * delta
	if uniforms.radius > limit or uniforms.radius < 0 then
		rate = -rate
	end
end
