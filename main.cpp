#include <string>
#include <vector>
#include <map>
#include <exception>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <boost/program_options.hpp>

extern "C"{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include "Vector4.hpp"

#include "clo_stream.h"

//const std::string font_filename("PROGGYCLEANSZ.FON");
const std::string font_filename("BMcube.TTF");

std::map<std::string, sf::Image> image_cache;

bool process_uniform_number(lua_State* lua_interp, sf::Shader& shader, std::string& uniform_name){
	// value is a number, set it as a parameter immediately
	// STACK: ... [TABLE uniforms] [STRING key] [NUMBER value]
	
	//std::cout << uniform_name << ": " << lua_tonumber(lua_interp, -1);
	
	shader.SetParameter(uniform_name, float(lua_tonumber(lua_interp, -1)));

	return true;
}

bool process_uniform_table(lua_State* lua_interp, sf::Shader& shader, std::string& uniform_name, int value_index){
	// standard table
	// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [NIL _v]

	// keep failed table lookup as key

	// 4 is max size of a vector
	float _v_contents[4] = {0.0};
	bool _v_filled[4] = {false};

	while (lua_next(lua_interp, value_index) != 0){
		// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [INDEX key] [DATA value]
		if (lua_type(lua_interp, -2) != LUA_TNUMBER){
			std::cerr << "lua uniform table requires numeric keys.\n";
		}
		if (lua_type(lua_interp, -1) != LUA_TNUMBER){
			std::cerr << "lua uniform table requires numeric values.\n";
		}
		// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [INDEX key] [NUMBER value]

		int _v_key = int(lua_tonumber(lua_interp, -2)) - 1;
		float _v_value = float(lua_tonumber(lua_interp, -1));
		if (_v_key < 0 || _v_key >= 4){
			std::cerr << "lua uniform table key '" << _v_key << "' out of range.\n";
		}
		if (_v_filled[_v_key] == true){
			std::cerr << "lua uniform table key '" << _v_key << "' is duplicate.\n";
		}
		_v_contents[_v_key] = _v_value;
		_v_filled[_v_key] = true;

		lua_pop(lua_interp, 1);
		// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [INDEX key]
	}
	// STACK: ... [TABLE uniforms] [STRING key] [TABLE value]

	lua_pushnil(lua_interp);
	// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [NIL]

	int _v_end = 0;
	for (size_t i = 0; i < 4; ++i){
		if (_v_filled[i]){
			if (_v_end != i){
				std::cerr << "lua uniform table contains gaps.\n";
			}
			++_v_end;
		}
	}
	switch (_v_end){
	case 1:
		shader.SetParameter(uniform_name, _v_contents[0]);
		break;
	case 2:
		shader.SetParameter(uniform_name, _v_contents[0], _v_contents[1]);
		break;
	case 3:
		shader.SetParameter(uniform_name, _v_contents[0], _v_contents[1], _v_contents[2]);
		break;
	case 4:
		shader.SetParameter(uniform_name, _v_contents[0], _v_contents[1], _v_contents[2], _v_contents[3]);
		break;
	default:
		std::cerr << "lua uniform table is invalid size.\n";
	}

	return true;
}

bool process_uniform_vector(lua_State* lua_interp, sf::Shader& shader, std::string& uniform_name, int value_index){
	// vecX/float table
	// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [TABLE _v]

	int _v_index = lua_gettop(lua_interp);
	lua_pushnil(lua_interp);
	// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [TABLE _v] [INDEX nil]

	// 4 is max size of a vector
	float _v_contents[4] = {0.0};
	bool _v_filled[4] = {false};

	while (lua_next(lua_interp, _v_index) != 0){
		// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [TABLE _v] [INDEX key] [DATA value]
		if (lua_type(lua_interp, -2) != LUA_TNUMBER){
			std::cerr << "lua uniform vector table requires numeric keys.\n";
		}
		if (lua_type(lua_interp, -1) != LUA_TNUMBER){
			std::cerr << "lua uniform vector table requires numeric values.\n";
		}
		// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [TABLE _v] [INDEX key] [NUMBER value]

		int _v_key = int(lua_tonumber(lua_interp, -2)) - 1;
		float _v_value = float(lua_tonumber(lua_interp, -1));
		if (_v_key < 0 || _v_key >= 4){
			std::cerr << "lua uniform vector key '" << _v_key << "' out of range.\n";
		}
		if (_v_filled[_v_key] == true){
			std::cerr << "lua uniform vector key '" << _v_key << "'is duplicate.\n";
		}
		_v_contents[_v_key] = _v_value;
		_v_filled[_v_key] = true;

		lua_pop(lua_interp, 1);
		// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [TABLE _v] [INDEX key]
	}
	// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [TABLE _v]

	int _v_end = 0;
	for (size_t i = 0; i < 4; ++i){
		if (_v_filled[i]){
			if (_v_end != i){
				std::cerr << "lua uniform vector contains gaps.\n";
			}
			++_v_end;
		}
	}
	switch (_v_end){
	case 1:
		//std::cout << "[" << _v_contents[0] << "]\n";
		shader.SetParameter(uniform_name, _v_contents[0]);
		break;
	case 2:
		//std::cout << "[" << _v_contents[0] << ", " << _v_contents[1] << "]\n";
		shader.SetParameter(uniform_name, _v_contents[0], _v_contents[1]);
		break;
	case 3:
		//std::cout << "[" << _v_contents[0] << ", " << _v_contents[1] << ", " << _v_contents[2] <<  "]\n";
		shader.SetParameter(uniform_name, _v_contents[0], _v_contents[1], _v_contents[2]);
		break;
	case 4:
		//std::cout << "[" << _v_contents[0] << ", " << _v_contents[1] << ", " << _v_contents[2] << ", " << _v_contents[3] << "]\n";
		shader.SetParameter(uniform_name, _v_contents[0], _v_contents[1], _v_contents[2], _v_contents[3]);
		break;
	default:
		std::cerr << "lua uniform vector is invalid size.\n";
	}

	//lua_pop(lua_interp, 1);
	//// STACK: ... [TABLE uniforms] [STRING key] [TABLE value]

	return true;
}

bool process_uniform_string(lua_State* lua_interp, sf::Shader& shader, std::string& uniform_name){
	// value is a string, load the image file and set as the sampler uniform
	// STACK: ... [TABLE uniforms] [STRING key] [STRING value]
	
	//std::cout << uniform_name << ": " << lua_tostring(lua_interp, -1);
	
	std::string filename = lua_tostring(lua_interp, -1);
	std::map<std::string, sf::Image>::iterator i = image_cache.find(filename);
	if (i == image_cache.end()) {
		image_cache[filename].LoadFromFile(filename);
	}
	shader.SetTexture(uniform_name, image_cache[filename]);

	return true;
}

bool process_uniforms(lua_State* lua_interp, sf::Shader& shader){
	// Set shader parameters (uniforms)
	// STACK: ... 
	lua_getglobal(lua_interp, "uniform");
	// STACK: ... [DATA uniforms]

	if (!lua_istable(lua_interp, -1)){
		std::cerr << "lua variable 'uniform' must be a table of numbers, strings and/or vecXs\n";
	}
		
	// STACK: ... [TABLE uniforms]
	int uniform_index = lua_gettop(lua_interp);
	// Prepare stack with empty key
	lua_pushnil(lua_interp);
	// STACK: ... [TABLE uniforms] [INDEX nil]
	while (lua_next(lua_interp, uniform_index) != 0){
		// STACK: ... [TABLE uniforms] [INDEX key] [DATA value]
		if (lua_type(lua_interp, -2) != LUA_TSTRING){
			std::cerr << "lua table uniform requires string keys\n";
		}
		// STACK: ... [TABLE uniforms] [STRING key] [DATA value]
			
		// The key is the name of the uniform variable
		std::string uniform_name(lua_tostring(lua_interp, -2));

		int value_index = lua_gettop(lua_interp);

		switch (lua_type(lua_interp, value_index)){
		case LUA_TNUMBER:
			process_uniform_number(lua_interp, shader, uniform_name);

			break;
		case LUA_TTABLE:
			lua_pushstring(lua_interp, "_v");
			// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [STRING _v]
			lua_gettable(lua_interp, -2);
			// STACK: ... [TABLE uniforms] [STRING key] [TABLE value] [DATA _v]
			if (lua_isnil(lua_interp, -1)){
				process_uniform_table(lua_interp, shader, uniform_name, value_index);

			}else{
				process_uniform_vector(lua_interp, shader, uniform_name, value_index);
			}
			lua_pop(lua_interp, 1);
			// STACK: ... [TABLE uniforms] [STRING key] [TABLE value]

			break;
		case LUA_TSTRING:
			process_uniform_string(lua_interp, shader, uniform_name);

			break;
		default:
			// STACK: ... [TABLE uniforms] [STRING key] [OTHER value]
			std::cerr << "Invalid uniform type '" << lua_typename(lua_interp, lua_type(lua_interp, -1)) << "' for key '" << lua_tostring(lua_interp, -2) << "'.\n";

			break;
		}

		// pop value, retain key as iterator
		lua_pop(lua_interp, 1);
		// STACK: ... [TABLE uniforms] [STRING key]
	}
	// STACK: ... [TABLE uniforms]
	lua_pop(lua_interp, 1);
	// STACK: ... 

	return true;
}

int main(int argc, char** argv){
	// Lua setup
	lua_State* lua_interp;

	lua_interp = lua_open();

	// Load lua subsystems
	luaL_openlibs(lua_interp);

	bool clo_fullscreen;
	unsigned int clo_width;
	unsigned int clo_height;
	unsigned int clo_bitdepth;
	std::string clo_image;
	//std::vector<std::string> clo_shader;
	std::string clo_shader;
	std::string clo_lua;

	sf::Vector2f clo_scale(1.f, 1.f);
	float clo_rotation;

	sf::Vector3f clo_background(0.f, 0.f, 0.f);

	sf::RenderWindow mainwin;

	sf::Sprite sprite;
	sf::Sprite spriteorig;

	sf::Image image;

	//std::vector<sf::Shader> shader;
	sf::Shader shader;

	std::vector<sf::Image> textures;

	try{
		boost::program_options::options_description clo_desc(std::string("Usage: ") + argv[0] + " image shader [--flags]");
		clo_desc.add_options()
			("image", boost::program_options::value<std::string>(&clo_image)->default_value(""), "sample image to apply shader to")
			//("shader", boost::program_options::value<std::vector<std::string> >(&clo_shader)->multitoken(), "shaders to be applied, in order")
			("shader", boost::program_options::value<std::string>(&clo_shader)->default_value(""), "shader to be applied")
			("help", "display help information")
			("fullscreen,f", boost::program_options::bool_switch(&clo_fullscreen)->default_value(false), "run in fullscreen mode")
			("width,w", boost::program_options::value<unsigned int>(&clo_width)->default_value(640), "set window width")
			("height,h", boost::program_options::value<unsigned int>(&clo_height)->default_value(480), "set window height")
			("bitdepth,b", boost::program_options::value<unsigned int>(&clo_bitdepth)->default_value(32), "set bit depth")
			("float,f", boost::program_options::value<std::vector<std::pair<std::string, float> > >()->composing()->multitoken(), "uniform float arguments passed to shader")
			("vec2,2", boost::program_options::value<std::vector<std::pair<std::string, sf::Vector2f> > >()->composing()->multitoken(), "uniform vec2 arguments passed to shader")
			("vec3,3", boost::program_options::value<std::vector<std::pair<std::string, sf::Vector3f> > >()->composing()->multitoken(), "uniform vec3 arguments passed to shader")
			("vec4,4", boost::program_options::value<std::vector<std::pair<std::string, sf::Vector4f> > >()->composing()->multitoken(), "uniform vec4 arguments passed to shader")
			("sampler,s", boost::program_options::value<std::vector<std::pair<std::string, std::string> > >()->composing()->multitoken(), "uniform sampler2D arguments passed to shader")
			("background,c", boost::program_options::value<sf::Vector3f>(&clo_background), "background color")
			("scale,s", boost::program_options::value<sf::Vector2f>(&clo_scale), "set sprite scale")
			("rotation,r", boost::program_options::value<float>(&clo_rotation)->default_value(0.f), "set sprite rotation")
			("lua,l", boost::program_options::value<std::string>(&clo_lua)->default_value("lua/default.lua"), "set sprite rotation")
		;

		boost::program_options::positional_options_description clo_posdesc;
		clo_posdesc.add("image", 1).add("shader", 1);

		boost::program_options::variables_map clo_map;
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(clo_desc).positional(clo_posdesc).run(), clo_map);
		boost::program_options::notify(clo_map);
	

		if (clo_map.count("help")){
			std::cout << clo_desc << "\n";
			return 1;
		}

		mainwin.Create(sf::VideoMode(clo_width, clo_height, clo_bitdepth), "ShaderPreview", sf::Style::Default);
	
		if (clo_image == ""){
			//ERROR no image specified
			std::cerr << "No image file specified\n";
			return 1;
		}
		
		if (!image.LoadFromFile(clo_image)){
			// ERROR image could not be opened/found
			std::cerr << "Could not find or load image file\n";
			return 1;
		}
		sprite.SetImage(image);
		spriteorig.SetImage(image);
		spriteorig.SetScale(clo_scale);
		sprite.SetScale(clo_scale);
		spriteorig.SetRotation(clo_rotation);
		sprite.SetRotation(clo_rotation);

		sprite.SetX(float(image.GetWidth() * spriteorig.GetScale().x));

		if (clo_shader.empty()){
			//ERROR no shader specified
			std::cerr << "No shader file specified\n";
			return 1;
		}

		if (!sf::Shader::IsAvailable()){
			//ERROR shaders not supported
			std::cerr << "Your graphics card does not support shaders\n";
			return 1;
		}
		
		if (!shader.LoadFromFile(clo_shader)){
			//ERROR shader could not be opened/found
			std::cerr << "Could not find or load shader file\n";
			return 1;
		}
		shader.SetCurrentTexture("image");


		if (clo_map.count("float")){
			std::vector<std::pair<std::string, float> > vp = clo_map["float"].as<std::vector<std::pair<std::string, float> > >();
			for (std::vector<std::pair<std::string, float> >::iterator i = vp.begin(); i != vp.end(); ++i){
				shader.SetParameter(i->first, i->second);
				std::cout << "float " << i->first << " = " << i->second << "\n";
			}
		}

		if (clo_map.count("vec2")){
			std::vector<std::pair<std::string, sf::Vector2f> > vp = clo_map["vec2"].as<std::vector<std::pair<std::string, sf::Vector2f> > >();
			for (std::vector<std::pair<std::string, sf::Vector2f> >::iterator i = vp.begin(); i != vp.end(); ++i){
				shader.SetParameter(i->first, i->second);
				std::cout << "vec2 " << i->first << " = [" << i->second.x << ", " << i->second.y << "]\n";
			}
		}

		if (clo_map.count("vec3")){
			std::vector<std::pair<std::string, sf::Vector3f> > vp = clo_map["vec3"].as<std::vector<std::pair<std::string, sf::Vector3f> > >();
			for (std::vector<std::pair<std::string, sf::Vector3f> >::iterator i = vp.begin(); i != vp.end(); ++i){
				shader.SetParameter(i->first, i->second);
				std::cout << "vec3 " << i->first << " = [" << i->second.x << ", " << i->second.y << ", " << i->second.z << "]\n";
			}
		}

		if (clo_map.count("vec4")){
			std::vector<std::pair<std::string, sf::Vector4f> > vp = clo_map["vec4"].as<std::vector<std::pair<std::string, sf::Vector4f> > >();
			for (std::vector<std::pair<std::string, sf::Vector4f> >::iterator i = vp.begin(); i != vp.end(); ++i){
				shader.SetParameter(i->first, i->second.x, i->second.y, i->second.z, i->second.w);
				std::cout << "vec4 " << i->first << " = [" << i->second.x << ", " << i->second.y << ", " << i->second.z << "]\n";
			}
		}

		if (clo_map.count("sampler")){
			std::vector<std::pair<std::string, std::string> > vp = clo_map["sampler"].as<std::vector<std::pair<std::string, std::string> > >();
			for (std::vector<std::pair<std::string, std::string> >::iterator i = vp.begin(); i != vp.end(); ++i){
				sf::Image texture;
				textures.push_back(texture);
				if (!textures.back().LoadFromFile(i->second)){
					// ERROR texture could not be opened/found
					std::cerr << "Could not find or load image file\n";
					return 1;
				}
				shader.SetTexture(i->first, textures.back());
				std::cout << "sampler " << i->first << " loaded\n";
			}
		}
	}catch(std::exception& e){
		std::cerr << e.what() << std::endl;
		return 1;
	}

	sf::Font font;
	if (!font.LoadFromFile(font_filename)){
		std::cerr << "Could not find or load " << font_filename << '\n';
	}
	sf::Text fps;
	fps.SetFont(font);
	fps.SetCharacterSize(8);
	fps.SetStyle(sf::Text::Regular);
	fps.SetColor(sf::Color::Red);

	if (luaL_dofile(lua_interp, clo_lua.c_str())){
		std::cerr << lua_tostring(lua_interp, -1);
		lua_pop(lua_interp, 1);
	}

	// Count how many frames have passed to use as a uniform
	unsigned int frame = 0;
	while(mainwin.IsOpened()){
		sf::Event ev;
        while (mainwin.PollEvent(ev)){
			if (ev.Type == sf::Event::Closed){
				mainwin.Close();
			}

            if ((ev.Type == sf::Event::KeyPressed) && (ev.Key.Code == sf::Key::Escape)){
                mainwin.Close();
			}

            if (ev.Type == sf::Event::Resized){
                //glViewport(0, 0, ev.Size.Width, ev.Size.Height);
			}
        }
		float delta = mainwin.GetFrameTime();

		// STACK: ...
		lua_getglobal(lua_interp, "main");
		// STACK: ... [DATA main]
		if (!lua_isfunction(lua_interp, -1)){
			std::cerr << "lua global value 'main' must be a function.\n";
		}
		// STACK: ... [FUNCTION main]
		lua_pushnumber(lua_interp, lua_Number(delta));
		// STACK: ... [FUNCTION main] [NUMBER delta]
		lua_call(lua_interp, 1, 0);
		// STACK: ...
		process_uniforms(lua_interp, shader);

		mainwin.Clear(sf::Color(int(255 * clo_background.x), int(255 * clo_background.y), int(255 * clo_background.z)));
		
		mainwin.Draw(spriteorig);
		mainwin.Draw(sprite, shader);

		// This feels very overboard
		std::stringstream ssfps;
		ssfps << int(1 / delta);

		fps.SetString(sf::String(ssfps.str()));

		fps.SetPosition(mainwin.GetWidth() - fps.GetRect().Width, 0.f);

		mainwin.Draw(fps);
        
        mainwin.Display();
	}

	lua_close(lua_interp);

	return 0;
}

