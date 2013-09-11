#ifndef _NAIVE_BAYES_CLASSIFIER_FEATURES_H
#define _NAIVE_BAYES_CLASSIFIER_FEATURES_H

#include <set>
#include <string>
#include <fstream>

// import features from "features file"
// line by line, one line for one word as feature
class Features {
 public:
	// initialize: ifstream
	explicit Features(std::ifstream& is);

	// word is feature ?
	bool IsFeatures(const std::string& word);

	// feature size
	size_t GetFeaturesSize() const;

 private:
	Features();
	//  Features(Features& );
	//  Features& operator=(const Features& ); 

 private:
	std::set<std::string> features_;
};

#endif
