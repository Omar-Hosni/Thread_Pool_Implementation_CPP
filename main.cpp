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
	threadsafe_queue<function_wrapper> work_queue;
	vector<thread> threads;
	join_threads joiner;

	void worker_thread()
	{
		while (done)
		{
			function_wrapper task;
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
	future<typename std::result_of<Function_type()>::type> submit(Function_type f)
	{
		typedef typename std::result_of<Function_type()>::type result_type;
		package_task<result_type()> task(move(f));
		future<result_type> res(task.get_future());
		work_queue.push(move(task));
		return res;
	}

	void run_pending_task()
	{
		function_wrapper task;
		if (work_queue.try_pop(task))
		{
			task();
		}
		else
		{
			this_thread::yield();
		}
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

