#include "features.h"

using namespace std;

Features::Features(std::ifstream& is) {
	std::string line;
	while (std::getline(is, line)) {
		features_.insert(line);
	}
}

bool Features::IsFeatures(const std::string& word) {
	if (features_.end() != features_.find(word)) 
		return true;
	else
		return false;
}

size_t Features::GetFeaturesSize() const {
	return features_.size();
}
