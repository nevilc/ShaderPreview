#ifndef H_CLO_STREAM

#include <iostream>
#include <string>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Vector4.hpp"

template<typename T>
std::istream& operator >>(std::istream& in, sf::Vector2<T>& pr){
	std::string s;
	std::stringstream ss;
	getline(in, s, ',');
	ss << s;
	ss >> pr.x;

	in >> pr.y;

	return in;
}

template<typename T>
std::istream& operator >>(std::istream& in, sf::Vector3<T>& pr){
	std::string s;
	std::stringstream ss;
	getline(in, s, ',');
	ss << s;
	ss >> pr.x;
	ss.clear();
	getline(in, s, ',');
	ss << s;
	ss >> pr.y;

	in >> pr.z;

	return in;
}

template<typename T>
std::istream& operator >>(std::istream& in, sf::Vector4<T>& pr){
	std::string s;
	std::stringstream ss;
	getline(in, s, ',');
	ss << s;
	ss >> pr.x;
	ss.clear();
	getline(in, s, ',');
	ss << s;
	ss >> pr.y;
	ss.clear();
	getline(in, s, ',');
	ss << s;
	ss >> pr.z;

	in >> pr.w;

	return in;
}

template<typename T>
std::istream& operator >>(std::istream& in, std::pair<std::string, T>& pr){
	getline(in, pr.first, '=');

	in >> pr.second;

	return in;
}

#endif//H_CLO_STREAM
