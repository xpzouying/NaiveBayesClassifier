#ifndef _NAIVE_BAYES_CLASSIFIER_TRAINDATA_H
#define _NAIVE_BAYES_CLASSIFIER_TRAINDATA_H

// 用来存放训练样本的数据元：
// 存放训练样本的文件名， 训练样本的类型

#include <string>

struct TrainData {
	std::string trainfile;
	std::string traintype;
};

#endif
