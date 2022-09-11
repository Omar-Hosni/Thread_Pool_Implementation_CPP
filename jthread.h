#include "jthread.h"
#include <thread>
#include <atomic>
#include <future>

using namespace std;

class interrupt_flag {

	bool local_flag;
public:
	void set() {
		local_flag = true;
	}
	bool is_set() {
		return local_flag;
	}
};

thread_local interrupt_flag this_thread_flag;

class jthread_local {
	thread _thread;
	interrupt_flag* flag;

public:
	template<typename FunctionType>
	jthread_local(FunctionType f) {

		promise<interrupt_flag*>p;
		_thread = thread([f, &p] {
			p.set_value(&this_thread_flag);
			f();
			});
	}

	void interrupt() {
		flag->set();
	}

	~jthread_local() {
		if (_thread.joinable()) {
			_thread.join();
		}
	}
};


bool interrupt_point() {
	if (this_thread_flag.is_set())
		return true;
	return false;
}
