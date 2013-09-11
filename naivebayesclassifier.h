#ifndef _NAIVE_BAYES_CLASSIFIER_NAIVEBAYESCLASSIFIER_H
#define _NAIVE_BAYES_CLASSIFIER_NAIVEBAYESCLASSIFIER_H

#include <string>
#include <vector>
#include <memory>

class Features;
class FeaturesRatio;
class Log;
// 键值对：存放类型和比例。naive bayes分类取概率最大的类型
struct ClusterTypeRatioPair {
	bool operator<(const ClusterTypeRatioPair& other) const {
		return proability > other.proability;
	}

	std::string type;
	double proability;
};

class NaiveBayesClassifier {
 public:
	//  NaiveBayesClassifier(const Features& features);
	NaiveBayesClassifier(std::shared_ptr<Features> spfeatures);

	// 初始化特征先验概率
	bool InitFeaturesRatio(const std::vector<FeaturesRatio>& vec_ratio);

	// 得到各种类型概率， 按照概率降序排列
	// ! NOTE: segmented_string : 分好词后的字符串
	std::vector<ClusterTypeRatioPair> GetClustersProability(const std::string& segmented_string);

 private:
	// 得到当前分类的概率： 使用朴素贝叶斯，最大似然估计
	// IN: 计算segmented_string在 featuresratio 类分类中的概率
	// NOTE: 为了节约效率。 FeaturesRatio为引用传入，会在查询单词频率的时候，修改其值
	double GetLogConditionalProability_(FeaturesRatio& clusterratio, const std::string& segmented_string);


	// Testcase --> add log
	// log will change
	double GetLogConditionalProability_(FeaturesRatio& clusterratio, const std::string& segmented_string, Log& log);


 private:
	//  std::vector<ClusterTypeRatioPair> vec_cluster_typeratio_;
	std::vector<FeaturesRatio> vec_ratio_; // n个独立条件概率

	std::shared_ptr<Features> spfeatures_;  // 特征字典 
	size_t sum_all_clusters_number_; // 统计所有类型的概率， 用于计算先验概率
};

#endif






