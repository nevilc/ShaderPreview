require 'glsl'

shader = {
	_file = nil,
	_id = nil,
	uniforms = {},
	
	load = function(filename)
		if _id ~= nil then
			return false
		end
		_file = filename
		_id = _extern_load_shader(filename)
		return true
	end,
	
	
}
	