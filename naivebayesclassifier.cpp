#include "naivebayesclassifier.h"

#include "tools.h"
#include "features.h"
#include "featuresratio.h"

#include <cmath>
#include <algorithm>

#include "log.h"

using namespace std;

NaiveBayesClassifier::NaiveBayesClassifier(shared_ptr<Features> spfeatures)
	: spfeatures_(spfeatures), 
	  sum_all_clusters_number_(0) {
}


// 初始化特征先验概率
bool NaiveBayesClassifier::InitFeaturesRatio(const std::vector<FeaturesRatio>& vec_ratio) {
	// 0和1类，没有必要分类
	if (vec_ratio.size() <= 1) return false;

	vec_ratio_ = vec_ratio;

	// 统计所有类型样本库的总和
	vector<FeaturesRatio>::const_iterator iter_end = vec_ratio_.cend();
	for (std::vector<FeaturesRatio>::const_iterator it_ratio = vec_ratio_.cbegin(); it_ratio != iter_end; ++it_ratio)
		sum_all_clusters_number_ += it_ratio->SumTrainData();

	return true;
}

// 得到各种类型及其概率， 按照概率降序排列
std::vector<ClusterTypeRatioPair> NaiveBayesClassifier::GetClustersProability(const std::string& segmented_string) {
	std::vector<ClusterTypeRatioPair> vec_typeratio;

	ofstream ofslog("log.log", ios::app);
	ofslog<<segmented_string<<endl;

	// 遍历所有的类型条件概率
	vector<FeaturesRatio>::iterator iter_end = vec_ratio_.end();
	for (std::vector<FeaturesRatio>::iterator it_ratio = vec_ratio_.begin(); it_ratio != iter_end; ++it_ratio) {
		ClusterTypeRatioPair typeratio_pair;
		typeratio_pair.type = it_ratio->Type(); // 类型


		// log --- TestCase
		Log testcase_log(ofslog, it_ratio->Type());
	  

		double clusterproability = ((double)it_ratio->SumTrainData() / (double)sum_all_clusters_number_); // 类型/所有类型 的先验概率

		testcase_log.Write()<<"ClusterProability = "<<clusterproability;

		double cond_proability = GetLogConditionalProability_(*it_ratio, segmented_string, testcase_log); // 独立条件概率
		//double cond_proability = GetLogConditionalProability_(*it_ratio, segmented_string); // 独立条件概率

		// 朴素贝叶斯概率估计 = log(类型概率) + log(先验概率)
		typeratio_pair.proability = log(clusterproability) + cond_proability;
		vec_typeratio.push_back(typeratio_pair);


		// Log for Testcase 
		testcase_log.Write()<<"Proability = "<<typeratio_pair.proability<<endl;
	}

	// here, already push all <type, proability> pair

	// sorted
	std::sort(vec_typeratio.begin(), vec_typeratio.end());

	return vec_typeratio;
}

// private function

// 得到当前分类的概率： 使用朴素贝叶斯，最大似然估计
// IN: 计算segmented_string在 featuresratio 类分类中的概率
double NaiveBayesClassifier::GetLogConditionalProability_(FeaturesRatio& clusterratio, const std::string& segmented_string) {
	// 分好词后的 单词列表
	std::vector<std::string> vec_words;
	string lower_segmented_string(segmented_string); // lower string
	StringLower(lower_segmented_string);
	StringSplitByChar(lower_segmented_string, ' ', &vec_words);

	double proability = 0;  // 使用log()统计，若使用乘法，则初始化proability = 1;
	// 对所有单词出现的概率进行统计
	vector<string>::const_iterator iter_end = vec_words.cend();
	for (vector<string>::const_iterator it_word = vec_words.cbegin();
		 it_word != iter_end;
		 ++ it_word) {
		if (it_word->empty()) 
			continue;

		if (spfeatures_->IsFeatures(*it_word)) { // word in features, compute
			// 朴素贝叶斯统计
			double single_word_proability = clusterratio.GetWordRatio(*it_word);
			proability += log(single_word_proability);
		}
	}// end for vec_words

	return proability;
}


// 得到当前分类的概率： 使用朴素贝叶斯，最大似然估计
// IN: 计算segmented_string在 featuresratio 类分类中的概率
double NaiveBayesClassifier::GetLogConditionalProability_(FeaturesRatio& clusterratio, const std::string& segmented_string, Log &testcase_log) {
	// 分好词后的 单词列表
	std::vector<std::string> vec_words;
	string lower_segmented_string(segmented_string); // lower string
	StringLower(lower_segmented_string);
	StringSplitByChar(lower_segmented_string, ' ', &vec_words);

	double proability = 0;  // 使用log()统计，若使用乘法，则初始化proability = 1;
	// 对所有单词出现的概率进行统计
	vector<string>::const_iterator iter_end = vec_words.cend();
	for (vector<string>::const_iterator it_word = vec_words.cbegin();
		 it_word != iter_end;
		 ++ it_word) {
		if (it_word->empty()) 
			continue;

		if (spfeatures_->IsFeatures(*it_word)) { // word in features, compute
			// 朴素贝叶斯统计
			double single_word_proability = clusterratio.GetWordRatio(*it_word);
			proability += log(single_word_proability);

			
			testcase_log.Write()<<*it_word<<":"<<single_word_proability;
		}
	}// end for vec_words

	return proability;
}
