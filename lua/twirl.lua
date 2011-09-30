require 'glsl'

uniforms = {
	rotation = 0
}

limit = 5.0
rate = 2.5

function main(delta)
	uniforms.rotation = uniforms.rotation + rate * delta
	if uniforms.rotation > limit or uniforms.rotation < -limit then
		rate = -rate
	end
end
