// See https://www.modernescpp.com/index.php/a-concise-introduction-to-coroutines-by-dian-lun-li/
// and https://www.modernescpp.com/index.php/coroutines-a-scheduler-for-tasks-by-dian-lun-li/
export module coroutines:taskscheduler;
import std;

namespace TaskScheduler
{
	// stackScheduler.cpp
	struct Task
	{
		struct promise_type
		{
			std::suspend_always initial_suspend() noexcept { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }

			Task get_return_object()
			{
				return std::coroutine_handle<promise_type>::from_promise(*this);
			}
			void return_void() {}
			void unhandled_exception() {}
		};

		Task(std::coroutine_handle<promise_type> handle) : handle{ handle } {}

		auto get_handle() { return handle; }

		std::coroutine_handle<promise_type> handle;
	};

	class Scheduler
	{
		std::stack<std::coroutine_handle<>> _tasks;

	public:
		void emplace(std::coroutine_handle<> task)
		{
			_tasks.push(task);
		}

		void schedule()
		{
			while (!_tasks.empty())
			{
				auto task = _tasks.top();
				_tasks.pop();
				task.resume();

				if (!task.done())
				{
					_tasks.push(task);
				}
				else
				{
					task.destroy();
				}
			}
		}

		auto suspend()
		{
			return std::suspend_always{};
		}
	};


	Task TaskA(Scheduler& sch)
	{
		std::cout << "Hello from TaskA\n";
		co_await sch.suspend();
		std::cout << "Executing the TaskA\n";
		co_await sch.suspend();
		std::cout << "TaskA is finished\n";
	}

	Task TaskB(Scheduler& sch)
	{
		std::cout << "Hello from TaskB\n";
		co_await sch.suspend();
		std::cout << "Executing the TaskB\n";
		co_await sch.suspend();
		std::cout << "TaskB is finished\n";
	}

	void Run()
	{
		std::cout << '\n';

		Scheduler sch;

		sch.emplace(TaskA(sch).get_handle());
		sch.emplace(TaskB(sch).get_handle());

		std::cout << "Start scheduling...\n";

		sch.schedule();

		std::cout << '\n';
	}
}

namespace QueueScheduler
{
	// queueScheduler.cpp
	struct Task
	{
		struct promise_type
		{
			std::suspend_always initial_suspend() noexcept { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }

			Task get_return_object()
			{
				return std::coroutine_handle<promise_type>::from_promise(*this);
			}
			void return_void() {}
			void unhandled_exception() {}
		};

		Task(std::coroutine_handle<promise_type> handle) : handle{ handle } {}

		auto get_handle() { return handle; }

		std::coroutine_handle<promise_type> handle;
	};

	class Scheduler
	{
		std::queue<std::coroutine_handle<>> _tasks;

	public:
		void emplace(std::coroutine_handle<> task)
		{
			_tasks.push(task);
		}

		void schedule()
		{
			while (!_tasks.empty())
			{
				auto task = _tasks.front();
				_tasks.pop();
				task.resume();

				if (!task.done())
				{
					_tasks.push(task);
				}
				else
				{
					task.destroy();
				}
			}
		}

		auto suspend()
		{
			return std::suspend_always{};
		}
	};

	Task TaskA(Scheduler& sch)
	{
		std::cout << "Hello from TaskA\n";
		co_await sch.suspend();
		std::cout << "Executing the TaskA\n";
		co_await sch.suspend();
		std::cout << "TaskA is finished\n";
	}

	Task TaskB(Scheduler& sch)
	{
		std::cout << "Hello from TaskB\n";
		co_await sch.suspend();
		std::cout << "Executing the TaskB\n";
		co_await sch.suspend();
		std::cout << "TaskB is finished\n";
	}

	void Run()
	{
		std::cout << '\n';

		Scheduler sch;

		sch.emplace(TaskA(sch).get_handle());
		sch.emplace(TaskB(sch).get_handle());

		std::cout << "Start scheduling...\n";

		sch.schedule();

		std::cout << '\n';
	}
}

namespace PriorityScheduler
{
	// Adapted from https://www.modernescpp.com/index.php/a-priority-scheduler-for-coroutines/
	// priority_queueSchedulerSimplified.cpp
	struct Task 
	{
		struct promise_type 
		{
			std::suspend_always initial_suspend() noexcept { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }

			Task get_return_object() 
			{
				return std::coroutine_handle<promise_type>::from_promise(*this);
			}
			void return_void() {}
			void unhandled_exception() {}
		};

		Task(std::coroutine_handle<promise_type> handle) : handle{ handle } {}

		auto get_handle() { return handle; }

		std::coroutine_handle<promise_type> handle;
	};

	class Scheduler 
	{
		std::priority_queue<std::pair<int, std::coroutine_handle<>>> _prioTasks;

		public:
			void emplace(int prio, std::coroutine_handle<> task) 
			{
				_prioTasks.push(std::make_pair(prio, task));
			}

			void schedule() 
			{
				while (!_prioTasks.empty()) 
				{
					auto [prio, task] = _prioTasks.top();
					_prioTasks.pop();
					task.resume();

					if (!task.done()) 
						_prioTasks.push(std::make_pair(prio, task));
					else 
						task.destroy();
				}
			}
	};

	Task createTask(const std::string& name)
	{
		std::cout << name << " start\n";
		co_await std::suspend_always();
		std::cout << name << " execute\n";
		co_await std::suspend_always();
		std::cout << name << " finish\n";
	}

	void Run()
	{
		std::cout << '\n';

		std::string taskA = "TaskA";
		std::string taskB = "TaskB";

		Scheduler scheduler1;

		scheduler1.emplace(0, createTask(taskA).get_handle());
		scheduler1.emplace(1, createTask(taskB).get_handle());

		scheduler1.schedule();

		std::cout << '\n';

		Scheduler scheduler2;

		scheduler2.emplace(1, createTask(taskA).get_handle());
		scheduler2.emplace(0, createTask(taskB).get_handle());

		scheduler2.schedule();

		std::cout << '\n';
	}
}

namespace AdvancedPriorityScheduler
{
	// https://www.modernescpp.com/index.php/an-advanced-priority-scheduler-for-coroutines/
	// https://www.modernescpp.com/index.php/a-bug-in-the-priority-scheduler-for-coroutines/
	struct Task 
	{
		struct promise_type 
		{
			std::suspend_always initial_suspend() noexcept { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }

			Task get_return_object() 
			{
				return std::coroutine_handle<promise_type>::from_promise(*this);
			}
			void return_void() {}
			void unhandled_exception() {}
		};

		Task(std::coroutine_handle<promise_type> handle) : handle{ handle } {}

		auto get_handle() { return handle; }

		std::coroutine_handle<promise_type> handle;
	};

	using job = std::pair<int, std::coroutine_handle<>>;

	template <typename Updater = std::identity,                         // (1)
		typename Comperator = std::ranges::less>
		requires std::invocable<decltype(Updater()), int>&&                // (2)
	std::predicate<decltype(Comperator()), job, job>
	class Scheduler 
	{
		std::priority_queue<job, std::vector<job>, Comperator> _prioTasks;

		public:
			void emplace(int prio, std::coroutine_handle<> task) 
			{
				_prioTasks.push(std::make_pair(prio, task));
			}

			void schedule() 
			{
				Updater upd = {};                                            // (3)
				while (!_prioTasks.empty()) 
				{
					auto [prio, task] = _prioTasks.top();
					_prioTasks.pop();
					task.resume();

					if (!task.done()) 
						_prioTasks.push(std::make_pair(upd(prio), task));          // (4)
					else 
						task.destroy();
				}
			}
	};

	Task createTask(const std::string& name) 
	{
		std::cout << name << " start\n";
		co_await std::suspend_always();
		for (int i = 0; i <= 3; ++i) 
		{
			std::cout << name << " execute " << i << "\n";                  // (5)
			co_await std::suspend_always();
		}
		co_await std::suspend_always();
		std::cout << name << " finish\n";
	}

	void Run() 
	{
		std::cout << '\n';

		std::string taskA = "TaskA";                    // (1)
		std::string taskB = "  TaskB";                  // (2)
		std::string taskC = "    TaskC";                // (3)

		Scheduler scheduler1;                                               // (6)

		scheduler1.emplace(0, createTask(taskA).get_handle());
		scheduler1.emplace(1, createTask(taskB).get_handle());
		scheduler1.emplace(2, createTask(taskC).get_handle());

		scheduler1.schedule();

		std::cout << '\n';

		Scheduler<decltype([](int a) { return a - 1; }) > scheduler2;        // (7)

		scheduler2.emplace(0, createTask(taskA).get_handle());
		scheduler2.emplace(1, createTask(taskB).get_handle());
		scheduler2.emplace(2, createTask(taskC).get_handle());

		scheduler2.schedule();

		std::cout << '\n';
	}
}