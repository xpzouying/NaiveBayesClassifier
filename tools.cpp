
#include "tools.h"
#include <sstream>

using namespace std;

string& StringUpper(string& str) {
	transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

string& StringLower(string& str) {
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

// split string by char
void StringSplitByChar(const std::string& str, const char separator, std::vector<std::string>* vecstrings) {
	if (str.empty()) return;
	
	if ( ! vecstrings->empty())
	  vecstrings->clear();

	stringstream ss(str);
	string onestring;
	while (getline(ss, onestring, separator)) {
		Trim(onestring);
		if ( ! onestring.empty())
			vecstrings->push_back(onestring);
	}
}

