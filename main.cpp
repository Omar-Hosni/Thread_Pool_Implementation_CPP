#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <unordered_map>
#include <atomic>
#include "Header1.h"
#include "jthread.h"
#include <assert.h>
#include "threadsafe_queue.h"
#include <functional>

using namespace std;

class thread_pool
{
	std::atomic_bool done;
	threadsafe_queue<std::function<void()>> work_queue;
	vector<thread> threads;
	join_threads joiner;

	void worker_thread()
	{
		while (done)
		{
			std::function<void()> task;
			if (work_queue.try_pop(task))
			{
				task();
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}

public:
	thread_pool() :done(false), joiner(threads)
	{
		int const thread_count = thread::hardware_concurrency();

		try {
			for (int i = 0; i < thread_count; i++)
			{
				threads.push_back(thread(&thread_pool::worker_thread, this));
			}
		}
		catch (...)
		{
			done = true;
			throw;
		}
	}

	~thread_pool()
	{
		done = true;
	}

	template<typename Function_type>
	void submit(Function_type f)
	{
		work_queue.push(function<void()>(f));
	}

};


int main() {
	
	
	thread_pool pool;
	cout << "Testing threads pool" << endl;

	for (int i = 0; i < 100; i++)
	{
		pool.submit([=] {
			printf("%d printed by thread %d \n", i, this_thread::get_id());
			});
	}





}

