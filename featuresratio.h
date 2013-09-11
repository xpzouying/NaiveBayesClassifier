#ifndef _NAIVE_BAYES_CLASSIFIER_FEATURESRATIO_H
#define _NAIVE_BAYES_CLASSIFIER_FEATURESRATIO_H

//#include <map>
#include <istream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "traindata.h"

class Features;
// 用来统计文件出现的频率
class FeaturesRatio {
 public:
	// 构造函数： 输入*特征库*
	//FeaturesRatio(const Features& features);
	FeaturesRatio(std::shared_ptr<Features> spfeatures);
	// 输入： *训练样本* --> 2种方法
	bool InitTrainRatio(const TrainData& traindata, bool retrain_data);  // 训练结果

	// return map<>::feature -> value
	double GetWordRatio(const std::string& word);

	// sum of traindata 
	inline size_t SumTrainData() const { return sum_traindata_; }
	// traindata type
	inline std::string Type() const { return traindata_type_; }

 private:
	void CalculateFeatures_(const std::vector<std::string>& vecwords);

 private:
	double sum_traindata_; // 此样本总数
	std::string traindata_type_;	// type for this traindata

	std::unordered_map<std::string, size_t> map_features_; // <feature, count>
	size_t count_features_occurrences_;  // 特征出现的总次数, 也就是 map_features_::second 总和

	//Features features_;
	std::shared_ptr<Features> spfeatures_;
	size_t features_size_;  // features_.size();
	// 统计 features 出现的先验概率
	std::unordered_map<std::string, double> map_features_ratio_;
};

#endif
