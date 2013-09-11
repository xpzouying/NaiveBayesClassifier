#ifndef __NBC_TIMER_H__
#define __NBC_TIMER_H__

#include <ctime>				// timer
#include <string>
#include <iostream>

namespace nbc {

	class Timer {
	public:
	Timer(std::string name)
		: name_(name) {
			time(&begin_);
		}

		~Timer() {
			time_t end;
			time(&end);
			std::cout<<name_
					 <<" - Duration (sec): "
					 <<difftime(end, begin_)
					 <<std::endl;
		}

	private:
		std::string name_;
		time_t begin_;
	};
}


#endif
