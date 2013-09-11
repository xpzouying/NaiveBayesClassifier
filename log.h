#ifndef __NBC_LOG_H__
#define __NBC_LOG_H__

#include <fstream>
#include <sstream>
#include <string>

class Log {
 public:
	Log(std::ofstream& os, const std::string& name)
		: ofs_(os), name_(name) {
	}

	~Log() {
		ofs_<<name_<<" "
			<<oss_.str()
			<<std::endl;
	}


	std::ostringstream& Write() {
		oss_<<"\t";
		return oss_;
	}


 private:
	std::string name_;
	std::ostringstream oss_;
	
	std::ofstream& ofs_;
};


#endif
