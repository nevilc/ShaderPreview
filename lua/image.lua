require 'shader.lua'

image = {
	x = 0,
	y = 0,
	scale_x = 1,
	scale_y = 1,
	rotation = 0,
	
	load = function(filename)
		if _id ~= nil then
			return false
		end
		_file = filename
		_id = _extern_load_shader(filename)
		return true
	end