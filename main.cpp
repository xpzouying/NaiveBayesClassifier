#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "traindata.h"
#include "tools.h"
#include "featuresratio.h"
#include "naivebayesclassifier.h"
#include "timer.h"
#include "features.h"

using namespace std;

/* --- 
   NOTE: 
   1. <.configure file> has magic/special format.

*/


namespace {
	// failure ERROR
	const string kFailure_Error = "Execute Error!\n"
		"Execute File: nbc.o, Must have -c -t argument\n"
		"./nbc.o -c ./traindata.conf -t ./Testcase > Testcase.Result\n"
		"-c ./trainingdata.config: configure file for training file.\n"
		"-t testcase_file: testcase\n";

	// magic string/number for configure file
	const string kConfigure_MagicString = "trainingdata.conf";

	const unsigned int kConfigureArgumentsCount = 4;

	// configure file argument
	const string kConfigure_FeaturesDict = "FEATURES_DICT";
	const string kConfigure_TrainingDataTrain = "TRAININGDATA_TRAIN";
	const string kConfigure_TrainingDataPath = "TRAININGDATA_PATH";
	const string kConfigure_TrainingDataFile = "TRAININGDATA_FILE";
}

// 判断 str 是否以 other 开头
bool StringBeginWith_(const string& str, const string& other);

// get value from configure file line: 
string GetArguValueFromString_(const string& strline);

// load configure file
bool LoadConfigure_(const string& conffile, 
					string *pfeatures_dict,
					bool *train_data_flag,
					vector<TrainData> *pvec_traindata);


int main(int argc, char *argv[]) {

	string file_configure;		// configure filename
	string file_testcase;			// testcase filename

	if (argc != 5) {
		cout<<kFailure_Error<<endl;
		exit(1);
	} else {
		// if argument number is right
		string strargv_index1(argv[1]);
		string strargv_index3(argv[3]);

		if ((strargv_index1 == "-c") && (strargv_index3 == "-t")) {
			file_configure = argv[2];
			file_testcase = argv[4];
		} else if ((strargv_index3 == "-c") && (strargv_index1 == "-t")) {
			file_testcase = argv[2];	  
			file_configure = argv[4];
		} else {
			cout<<kFailure_Error<<endl;
			exit(1);
		}
	} // end if (argc != 5)

	Trim(file_configure);
	Trim(file_testcase);

	// read 3 configure argument: features dict, training data path, training data file
	string features_dict; // dictionary file
	bool retrain_data = false;  // train data?
	vector<TrainData> vec_traindata; // training data element

	// read configure 
	bool load_configure_result = LoadConfigure_(file_configure, &features_dict, &retrain_data, &vec_traindata);
	if ( ! load_configure_result) return 1; // error 
	Trim(features_dict);

	// 1. features
	ifstream featuresdict(features_dict);
	if ( ! featuresdict.is_open() ) {
		cout<<"features dictionary open error!"<<endl;
		return 1;
	}

	//Features features(featuresdict);
	shared_ptr<Features> spfeatures(new Features(featuresdict));

	// 2. Features Ratio
	vector<FeaturesRatio> vec_featuresratio; // 各个分类的统计次数

	// TrainData --> FeaturesRatio
	for (vector<TrainData>::const_iterator it_traindata = vec_traindata.begin(); it_traindata != vec_traindata.end(); ++it_traindata) {
		// 定义 和 初始化！必须
		FeaturesRatio newfeatureratio(spfeatures); // 特征字典
		bool result_trainratio = newfeatureratio.InitTrainRatio(*it_traindata, retrain_data); // 该类型的训练样本
		if ( ! result_trainratio) {	// init train data error
			cout<<"train ratio data error! "
				<<it_traindata->traintype
				<<", file is : "
				<<it_traindata->trainfile 
				<<endl; 
			return 1;
		}

		// generate FeaturesRatio Success. 
		vec_featuresratio.push_back(newfeatureratio);
	} // end for



	// 3. NaiveBayesClassifier
	NaiveBayesClassifier nbc(spfeatures);
	bool result_initfeaturesratio = nbc.InitFeaturesRatio(vec_featuresratio);
	if ( ! result_initfeaturesratio) {
		cout<<"NaiveBayesClassifier Init Featureratio Error!"<<endl;
		return 1;
	}



	{ // 4. Testcase
		nbc::Timer testcase_timer("Test Testcase");

		ifstream ifs_testcase(file_testcase);
		if ( ! ifs_testcase.is_open()) {
			cout<<"Testcase file cannot open!"<<endl;
			return 1;
		}

		// save cluster result
		string testcase_line;
		while(std::getline(ifs_testcase, testcase_line)) {

			StringLower(testcase_line);

			vector<ClusterTypeRatioPair> restyperatio = 
				nbc.GetClustersProability(testcase_line);

			// print proabilitys
			for (vector<ClusterTypeRatioPair>::iterator it_t_p = restyperatio.begin(); it_t_p != restyperatio.end(); ++it_t_p) {
				cout<<it_t_p->type<<":"<<it_t_p->proability<<", ";
			}

			cout<<"\t"<<testcase_line<<endl;
		} // while, end read testcase file

	} //end Test

	return 0;
}


///////////////////////////////////////////////////////////
// --- private function ---

// 判断 str 是否以 other 开头
bool StringBeginWith_(const string& str, const string& other) {
	bool compare_result = str.compare(0, other.length(), other);
	return compare_result == 0;
}

// get value from configure file line: 
string GetArguValueFromString_(const string& strline) {
	// "FEATURES_DICT = value";
	// "TRAININGDATA_PATH = value";
	// "TRAININGDATA_FILE = value";
	const string kToken = "=";

	return strline.substr(strline.find(kToken) + 1);
}

bool LoadConfigure_(const string& conffile, string *pfeatures_dict, bool *train_data_flag, vector<TrainData> *pvec_traindata) {
	ifstream ifs_configure(conffile.c_str()); // configure file
	if ( ! ifs_configure.is_open() ) {
		cout<<"open configure or testcase file error!"<<endl;
		return false;
	}

	string conf_string_line;
	std::getline(ifs_configure, conf_string_line);
	if (conf_string_line != kConfigure_MagicString) {
		// not magic string for configure
		cout<<"configure file format error! Need Magic String for configure."<<endl;
		return false;
	}

	string strtaindatas_path;		// 存放训练样本的目录
	vector<TrainData> vec_traindata; // 存放训练样本 filename & type

	// read 3 configure argument: features dict, training data path, training data file
	int readconf_argu_number = 0;
	while (std::getline(ifs_configure, conf_string_line)) {
		// is we need argument
		if (StringBeginWith_(conf_string_line, kConfigure_FeaturesDict)) {
			// "FEATURES_DICT"
			string featuresdict = GetArguValueFromString_(conf_string_line);
			Trim(featuresdict);

			*pfeatures_dict = featuresdict;
		} else if (StringBeginWith_(conf_string_line, kConfigure_TrainingDataTrain)) {
			// "TRAININGDATA_TRAIN"
			string trainflag = GetArguValueFromString_(conf_string_line);
			Trim(trainflag);
			StringLower(trainflag);

			*train_data_flag = (trainflag == "true") ? true : false;
		} else if (StringBeginWith_(conf_string_line, kConfigure_TrainingDataPath)) {
			// "TRAININGDATA_PATH"
			strtaindatas_path = GetArguValueFromString_(conf_string_line);
			Trim(strtaindatas_path);
		} else if (StringBeginWith_(conf_string_line, kConfigure_TrainingDataFile)) {
			// "TRAININGDATA_FILE"
			string str_trainfiletypes = GetArguValueFromString_(conf_string_line);
			const char kSplitTokenFiles = '\t'; // 多个文件之间
			const char kSplitTokenNameType = ':';
	  
			// 1. split files
			vector<string> vec_file_types;
			StringSplitByChar(str_trainfiletypes, kSplitTokenFiles, &vec_file_types);

			// 2. split file_name:file_type
			// vector element is -->  filename:type
			for (vector<string>::const_iterator it_filetype = vec_file_types.begin(); it_filetype != vec_file_types.end(); ++it_filetype) {
				// get token position
				size_t pos_token_nametype = it_filetype->find(kSplitTokenNameType);
				string thisfile = it_filetype->substr(0, pos_token_nametype);
				string thistype = it_filetype->substr(pos_token_nametype+1);

				Trim(thisfile);
				Trim(thistype);

				TrainData traindata;
				traindata.trainfile = thisfile;
				traindata.traintype = thistype;

				vec_traindata.push_back(traindata);
			}

		} else {  // not our need 
			continue;
		}

	

		// here, must be right argument
		if (++readconf_argu_number == kConfigureArgumentsCount) { // read all argument
			for (vector<TrainData>::const_iterator it_traindata = vec_traindata.begin(); it_traindata != vec_traindata.end(); ++it_traindata) {
				// full filename = path + filename
		
				TrainData traindata;
				traindata.trainfile = strtaindatas_path + it_traindata->trainfile;
				traindata.traintype = it_traindata->traintype;
				pvec_traindata->push_back(traindata);
			}

			return true;
		}
	} // end while 

	return false;
}
