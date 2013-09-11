#include "featuresratio.h"

#include <iostream>
#include <string>
#include <vector>

#include "tools.h"
#include "features.h"
#include "timer.h"

using namespace std;

namespace {
	const std::string kTrainDataSuffix = ".tr";
}

//FeaturesRatio::FeaturesRatio(const Features& features)
FeaturesRatio::FeaturesRatio(shared_ptr<Features> spfeatures)
	: sum_traindata_(0), 
	  count_features_occurrences_(0), 
	  spfeatures_(spfeatures), 
	  features_size_(spfeatures_->GetFeaturesSize()) {
}

bool FeaturesRatio::InitTrainRatio(const TrainData& traindata, bool retrain_data) {
	// train type
	traindata_type_ = traindata.traintype;
	nbc::Timer traintimer("InitTrainRatio() " + traindata_type_);
 
	if (retrain_data) { // 重新训练
		// train file name (include path)
		string trainfilename = traindata.trainfile;
		ifstream ifs_trainfile(trainfilename.c_str());
		if ( ! ifs_trainfile.is_open()) {
			cout<<"TrainData Init @InitTrainRatio() ERROR!"<<traindata_type_<<endl;
			return false;
		}

		vector<string> vecwords;  // 分词后，每一行中出现的单词
		string line;
		while (getline(ifs_trainfile, line)) { // 按行读取样本库
			Trim(line);
			if (line.empty()) continue;	

			StringLower(line);

			++ sum_traindata_; // count sample sum
			StringSplitByChar(line, ' ', &vecwords);

			// 统计每一行中特征出现的次数
			CalculateFeatures_(vecwords);
		}

		// SAVE TO FILE
		ofstream ofs_pretrainfile(traindata_type_ + kTrainDataSuffix);
		if ( ! ofs_pretrainfile.is_open()) {
			cout<<"TrainData Init @InitTrainRatio() ERROR! By Save type : "<<traindata_type_<<endl;
			return false;
		}


		// the first line is sum of traindata line 
		ofs_pretrainfile << sum_traindata_ <<endl;

		// the rest lines of file are <word --> proability>
		std::unordered_map<std::string, size_t>::const_iterator iter_end = map_features_.end();
		for (std::unordered_map<std::string, size_t>::const_iterator it = map_features_.cbegin();
			 it != iter_end;
			 ++ it) {
			ofs_pretrainfile << it->first 
							 << '\t'
							 <<it->second
							 <<endl;
		}

	} else { // 加载上一次的训练结果，结果文件保存在 类型同名的文件中
		ifstream ifs_pretrainfile(traindata_type_ + kTrainDataSuffix);
		if ( ! ifs_pretrainfile.is_open()) {
			cout<<"TrainData Init @InitTrainRatio() ERROR! By Load type : "<<traindata_type_<<endl;
			return false;
		}

		// read file line by line. The first line is sum of traindata line.
		string line;
		// 1. first line : sum of traindata lines
		if ( ! getline(ifs_pretrainfile, line) ) {
			cout<<"Get sum of traindata lines FAILED!"<<endl;
			return false;
		}
		sum_traindata_ = stoul(line);

		// 2. the rest of is <word --> proability>
		while (getline(ifs_pretrainfile, line)) {
			// 每一行为： word \t word_count
			vector<string> strvec;
			StringSplitByChar(line, '\t', &strvec);
			string word = strvec[0];
			size_t wordcount = stoul(strvec[1]);
	  
			map_features_.insert(make_pair(word, wordcount));

			// 累计每一个特征的次数
			count_features_occurrences_ += wordcount;
		}
	}

	return true;
}

// return map<>::feature -> value
double FeaturesRatio::GetWordRatio(const string& word) {
	// 查询一个，计算一个，存入map_features_ratio_中。
	// 如果 map_features_ratio_中 没有，再查看 map_features_ 进行计算，
	// 如果 map_features_还没有，就返回一个*值*（修正的）

	// 1. 在 map_features_ratio_ 中查询
	unordered_map<string, double>::iterator it_wordratio = map_features_ratio_.find(word);
	if (it_wordratio != map_features_ratio_.end()) {
		// 如果 map_features_ratio_ 存在， 直接返回
		return it_wordratio->second;
	}

	// 2. 如果 map_features_ratio_　不存在
	// 2.1 先去查找 map_features_ 中出现的值
	unordered_map<string, size_t>::iterator it_wordfeatures_value = map_features_.find(word);
	// features 出现的次数
	size_t wordfeatures_value = (it_wordfeatures_value != map_features_.end()) ?
		it_wordfeatures_value->second : 0;

	// 2.2 概率公式---  P(W_k|C_j) = (N_k + a) / (N + a*Vocabulary)
	// 假设a = 1， 拉普拉斯修正
	const double FIX_Pa = 1;
	// wordratio : 贝叶斯统计 , 条件概率
	double wordratio = (double)(wordfeatures_value + FIX_Pa) / (double)(count_features_occurrences_ + FIX_Pa * features_size_);

	// 2.3 插入到 map_features_ratio_ 中，供下次查询使用
	map_features_ratio_.insert(make_pair(word, wordratio));
  
	return wordratio;
}

////////////////////////////////
// --- privte ---
void FeaturesRatio::CalculateFeatures_(const vector<string>& vecwords) {
	vector<string>::const_iterator iter_vec_end = vecwords.end();
	for (vector<string>::const_iterator it_this_word = vecwords.begin();
		 it_this_word != iter_vec_end;
		 ++it_this_word) {

		if (it_this_word->empty()) continue;

		// if this word is feature
		if (spfeatures_->IsFeatures(*it_this_word)) {
			if (map_features_.end() != map_features_.find(*it_this_word)) {
				// 2.1 if in map<>, add value 
				++ map_features_.find(*it_this_word)->second;
			}
			else {
				// 2.2 if not in map<>, insert
				map_features_.insert(make_pair(*it_this_word, 1));
				// std::pair<string, int> new_feature(*it_this_word, 1);
				// map_features_.insert(new_feature);
			}
	  
			// increase feature occurrences
			++ count_features_occurrences_;
		} else {
			// if not a feature, ignore it.
		}

	}// end for
}
