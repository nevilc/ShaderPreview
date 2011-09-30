--[[
	Contains functions for creating GLSL-style vectors, including swizzling functionality.
	e.g.
		v = vec4(1.0, 2.0, 3.0, 4.0)
		-- v = {1.0, 2.0, 3.0, 4.0}
		vv = v.yz
		-- vv = {2.0, 4.0}
		v.yzw = vv.xyx
		-- v = {1.0, 2.0, 4.0, 2.0}
--]]

vec = {
	_v = {},
	
	_vec_op = function(self, other, op)
		-- Supports vector/vector and vector/scalar operations
		local l = {}
		
		if type(other) == 'number' then
			for i, val in ipairs(rawget(self, '_v')) do
				table.insert(l, op(val, other))
			end
		elseif type(other) == 'table' then
			if #self ~= #other then
				--error
			end
			
			for i, val in ipairs(rawget(self, '_v')) do
				table.insert(l, op(val, rawget(other, '_v')[i]))
			end
		end
		
		return _vec(#l, l)
	end,
	
	_new = function()
		local u = {_v = {}, _vec_op = rawget(vec, '_vec_op'), _new = rawget(vec, '_new'), _print = rawget(vec, '_print')}
		return setmetatable(u, getmetatable(vec))
	end,
	
	_print = function(self)
		local s = '['
		for i, val in ipairs(rawget(self, '_v')) do
			s = s .. val .. ', '
		end
		s = s .. ']'
		print(s)
	end
}

setmetatable(vec, {
	__len = function(self)
		return #rawget(self, '_v')
	end,

	__add = function(a, b)
		--print getmetatable({}).__add(2, 2)
		--return rawget(a, '_vec_op')(a, b, getmetatable(0).__add)
		return rawget(a, '_vec_op')(a, b, function(c, d) return c + d end)
	end,
	
	__sub = function(a, b)
		return rawget(a, '_vec_op')(a, b, function(c, d) return c - d end)
	end,
	
	__mul = function(a, b)
		return rawget(a, '_vec_op')(a, b, function(c, d) return c * d end)
	end,
	
	__div = function(a, b)
		return rawget(a, '_vec_op')(a, b, function(c, d) return c / d end)
	end,
	
	__mod = function(a, b)
		return rawget(a, '_vec_op')(a, b, function(c, d) return c % d end)
	end,
	
	__pow = function(a, b)
		return rawget(a, '_vec_op')(a, b, function(c, d) return c ^ d end)
	end,
	
	__unm = function(a)
		return rawget(a, '_vec_op')(a, -1, function(c, d) return c * d end)
	end,
	
	__concat = function(a, b)
		return vec(#a + #b, table.concat(rawget(a, '_v'), rawget(b, '_v')))
	end,
	
	__eq = function(a, b)
		if #a ~= #b then
			return false
		end
		
		for i, val in ipairs(rawget(a, '_v')) do
			if val ~= rawget(b, '_v')[i] then
				return false
			end
		end
		
		return true
	end,
	
	__index = function(t, key)
		if type(key) == 'number' then
			return rawget(t, '_v')[key]
		elseif type(key) == 'string' then
			if #key > 4 then
				--error
				print('error')
			end
			
			local l = {}
			local first = key:sub(1, 1)
			local chars = nil
			
			if first == 'r' or first == 'g' or first == 'b' or first == 'a' then
				chars = {'r', 'g', 'b', 'a'}
			elseif first == 'x' or first == 'y' or first == 'z' or first == 'b' then
				chars = {'x', 'y', 'z', 'w'}
			elseif first == 's' or first == 't' or first == 'p' or first == 'q' then
				chars = {'s', 't', 'p', 'a'}
			else
				--error
				print('error')
			end
			
			for c in key:gmatch"." do
				
				if c == chars[1] then
					table.insert(l, rawget(t, '_v')[1])
				elseif c == chars[2] then
					table.insert(l, rawget(t, '_v')[2])
				elseif c == chars[3] then
					table.insert(l, rawget(t, '_v')[3])
				elseif c == chars[4] then
					table.insert(l, rawget(t, '_v')[4])
				else
					--error
					print('error')
				end
			end
			
			return _vec(#l, l)
		else
			print('error')
		end
	end,
	
	__newindex = function(t, key, value)	
		if type(key) == 'number' then
			if type(value) ~= 'number' then
				print('error')
			end
			
			rawget(t, '_v')[key] = value
		elseif type(key) == 'string' then
			if #key > #rawget(t, '_v') or (type(value) == 'table' and #value > #rawget(t, '_v')) then
				--error
				print('error')
			end
		
			local chars = nil
			
			local first = key:sub(1, 1)
			if first == 'r' or first == 'g' or first == 'b' or first == 'a' then
				chars = {'r', 'g', 'b', 'a'}
			elseif first == 'x' or first == 'y' or first == 'z' or first == 'b' then
				chars = {'x', 'y', 'z', 'w'}
			elseif first == 's' or first == 't' or first == 'p' or first == 'q' then
				chars = {'s', 't', 'p', 'q'}
			else
				--error
				print('error')
			end
			
			print(first)
			
			local vindex = 0
			
			if type(value) == 'number' then
				--not implemented
				print('not implemented')
			elseif type(value) == 'table' then
				if rawget(value, '_v') then
					print(key)
					for c in key:gmatch('.') do
						print(c)
						vindex = vindex + 1
						if c == chars[1] then
							rawget(t, '_v')[1] = rawget(value, '_v')[vindex]
						elseif c == chars[2] then
							rawget(t, '_v')[2] = rawget(value, '_v')[vindex]
						elseif c == chars[3] then
							rawget(t, '_v')[3] = rawget(value, '_v')[vindex]
						elseif c == chars[4] then
							rawget(t, '_v')[4] = rawget(value, '_v')[vindex]
						else
							--error
							print('error')
						end
					end
				else
					for c in key:gmatch('.') do
						vindex = vindex + 1
						if c == chars[1] then
							rawget(t, '_v')[1] = value[vindex]
						elseif c == chars[2] then
							rawget(t, '_v')[2] = value[vindex]
						elseif c == chars[3] then
							rawget(t, '_v')[3] = value[vindex]
						elseif c == chars[4] then
							rawget(t, '_v')[4] = value[vindex]
						else
							--error
							print('error')
						end
					end
				end
			end
		else
			print('error')
		end
	end
})

_vec = function(size, ...)
	if size > 4 then
		--error
	end
	
	local arg = {...}
	
	if not #arg then
		--error
	end
	
	local pos = 0
	local apos = 1
	
	local v = rawget(vec, '_new')()
	
	while pos < size do
		if type(arg[apos]) == 'number' then
			table.insert(rawget(v, '_v'), arg[apos])
			pos = pos + 1
		elseif type(arg[apos]) == 'table' then
			for i, val in ipairs(arg[apos]) do
				table.insert(rawget(v, '_v'), val)
				pos = pos + 1
				if pos == size then
					break
				end
			end
		end
		apos = apos + 1
	end
	return v
end

float = function(...)
	return _vec(1, ...)
end

vec2 = function(...)
	return _vec(2, ...)
end

vec3 = function(...)
	return _vec(3, ...)
end

vec4 = function(...)
	return _vec(4, ...)
end
