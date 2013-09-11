/***
	file: tools.h, tools.cpp
***/

#ifndef _NAIVE_BAYES_TOOLS_H
#define _NAIVE_BAYES_TOOLS_H

#include <string>
#include <algorithm>
#include <sstream>
#include <vector>

// trim from start
inline std::string &LeftTrim(std::string &s) {
	s.erase(s.begin(), 
			std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
inline std::string &RightTrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
inline std::string &Trim(std::string &s) {
	return LeftTrim(RightTrim(s));
}


inline std::string IntToString(int value) {
	std::ostringstream oss;
	oss<<value;
	return std::string(oss.str());
}

inline int StringToInt(const std::string& s) {
	int intvalue;
	std::stringstream(s) >> intvalue;
	return intvalue;
}

inline size_t StringToSizet(const std::string& s) {
	size_t v;
	std::stringstream(s) >> v;
	return v;
}

// upper string
std::string& StringUpper(std::string& str);
// lower string
std::string& StringLower(std::string& str);

// split string by char. OUT: vecstrings
void StringSplitByChar(const std::string& str, const char separator, std::vector<std::string>* vecstrings);

#endif
