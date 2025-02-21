module;

#include <Windows.h>
#pragma comment(lib, "Synchronization.lib")

export module coroutines:random;
import std;

export namespace Coroutines::Random
{
	// Thread safe queue for coroutine handles
	struct Queue
	{
		void Add(std::coroutine_handle<>& handle)
		{
			std::scoped_lock lock{ m_mutex };
			m_cos.push_back(handle);
			m_semaphore.release();
		}

		void Add(std::coroutine_handle<>&& handle)
		{
			std::scoped_lock lock{ m_mutex };
			m_cos.push_back(std::move(handle));
			m_semaphore.release();
		}

		bool Get(std::coroutine_handle<>& out)
		{
			bool success = m_semaphore.try_acquire_for(std::chrono::seconds{1});
			if (not success)
				return false;
			std::scoped_lock lock{ m_mutex };
			if (m_cos.empty())
				return false;
			out = std::move(m_cos[0]);
			m_cos.erase(m_cos.begin());
			return true;
		}

	private:
		std::mutex m_mutex{};
		std::vector<std::coroutine_handle<>> m_cos;
		std::counting_semaphore<5> m_semaphore{0};
	};

	Queue WorkQueue{};

	template<bool TLaunch = true>
	struct awaitable
	{
		bool await_ready() { return false; }
		void await_suspend(std::coroutine_handle<> h)
		{
			if constexpr(TLaunch)
				WorkQueue.Add(h);
		}
		void await_resume()
		{
			std::println("AwaitResume(): {}", std::this_thread::get_id());
		}
	};

	struct Result
	{
		std::binary_semaphore signal{ 0 };
		int value = 0;
	};

	struct task
	{
		std::shared_ptr<Result> value;
		task(std::shared_ptr<Result> p) : value(p) { }

		int get()
		{
			value->signal.acquire();
			return value->value;
		}

		struct promise_type
		{
			std::shared_ptr<Result> ptr = std::make_shared<Result>();
			task get_return_object()
			{
				return { ptr };
			}
			std::suspend_never initial_suspend() { return {}; }
			std::suspend_never final_suspend() noexcept { return {}; }
			void unhandled_exception() {}
			void return_value(int i)
			{
				ptr->value = i;
				ptr->signal.release();
			}
		};
	};

	struct Thread
	{
		void Start()
		{
			m_thread = std::jthread{ &Thread::Run, this };
		}

		void SignalToExit()
		{
			m_run = false;
		}

	private:
		void Run()
		{
			while (m_run)
			{
				std::coroutine_handle<> h;
				if (WorkQueue.Get(h))
					h.resume();
			}
		}

		static constexpr std::chrono::seconds SleepTime{ 1 };
		bool m_run = true;
		std::jthread m_thread;
	};

	auto switch_to_new_thread()
	{
		return awaitable{};
	}

	std::default_random_engine engn;
	std::uniform_int_distribution<int> dist(1, 50);

	task resuming_on_new_thread()
	{
		co_await switch_to_new_thread();
		std::println("Resuming on worker thread...");
		co_return dist(engn);
	}

	void Run()
	{
		Thread t1{};
		Thread t2{};
		t1.Start();
		t2.Start();

		task x = resuming_on_new_thread();
		task y = resuming_on_new_thread();

		std::println("The x value is {}", x.get());
		std::println("The y value is {}", y.get());

		std::println("Finishing...");
		t1.SignalToExit();
		t2.SignalToExit();
		std::println("Bye...");
	}
}

export namespace Coroutines::WithFutex
{
	struct Queue
	{
		void Add(std::coroutine_handle<>& handle)
		{
			std::scoped_lock lock{ m_mutex };
			m_cos.push_back(handle);
			m_semaphore.release();
		}

		void Add(std::coroutine_handle<>&& handle)
		{
			std::scoped_lock lock{ m_mutex };
			m_cos.push_back(std::move(handle));
			m_semaphore.release();
		}

		bool Get(std::coroutine_handle<>& out)
		{
			bool success = m_semaphore.try_acquire_for(std::chrono::seconds{1});
			if (not success)
				return false;

			std::scoped_lock lock{ m_mutex };
			if (m_cos.empty())
				return false;
				
			out = std::move(m_cos[0]);
			m_cos.erase(m_cos.begin());
			return true;
		}

	private:
		std::mutex m_mutex{};
		std::vector<std::coroutine_handle<>> m_cos;
		std::counting_semaphore<5> m_semaphore{0};
	};

	Queue WorkQueue{};

	template<bool TLaunch = true>
	struct Awaitable
	{
		bool await_ready() { return false; }
		void await_suspend(std::coroutine_handle<> h)
		{
			if constexpr(TLaunch)
				WorkQueue.Add(h);
		}
		void await_resume()
		{
			std::println("AwaitResume(): {}", std::this_thread::get_id());
		}
	};

	struct Result
	{
		bool Done = false;
		int Number = 0;
	};

	struct Task
	{
		std::shared_ptr<Result> value;
		Task(std::shared_ptr<Result> p) : value(p) { }
		bool compare = false;

		int Get()
		{
			std::println("Getting.");
			while (not value->Done)
			{
				WaitOnAddress(
					&value->Done,
					&compare,
					sizeof(int),
					INFINITE
				);
			}

			return value->Number;
		}

		struct promise_type // name must be promise_type
		{
			~promise_type() { std::println("promise_type destroyed."); }
			std::shared_ptr<Result> ptr = std::make_shared<Result>();
			Task get_return_object()
			{
				// Can pass this back to Task. Don't pass the promise_type
				// instance back as it's destroyed by then.
				//std::coroutine_handle<promise_type>::from_promise(*this);
				return { ptr };
			}
			std::suspend_never initial_suspend() { return {}; }
			std::suspend_never final_suspend() noexcept { return {}; }
			void unhandled_exception() {}
			void return_value(int i)
			{
				ptr->Number = i;
				ptr->Done = true;
				WakeByAddressSingle(&ptr->Done);
			}
		};
	};

	struct Thread
	{
		void Start()
		{
			m_thread = std::jthread{ Thread::EntryPoint, this };
		}

		void SignalToExit()
		{
			m_run = false;
		}

		static void EntryPoint(Thread* self)
		{
			self->Run();
		}

		void Run()
		{
			while (m_run)
			{
				std::coroutine_handle<> h;
				if (WorkQueue.Get(h))
					h.resume();
				if (not m_run)
					break;
			}
		}

	private:
		static constexpr std::chrono::seconds SleepTime{ 1 };
		bool m_run = true;
		std::jthread m_thread;
	};

	auto SwitchToNewThread()
	{
		return Awaitable{};
	}

	std::default_random_engine engn;
	std::uniform_int_distribution<int> dist(1, 50);

	Task RunOnNewThread()
	{
		co_await SwitchToNewThread();
		std::println("Resuming on worker thread...");
		co_return dist(engn);
	}

	struct VoidTask
	{
		struct promise_type // name must be promise_type
		{
			VoidTask get_return_object()
			{
				return {};
			}
			std::suspend_never initial_suspend() { return {}; }
			std::suspend_never final_suspend() noexcept { return {}; }
			void unhandled_exception() {}
			void return_void() {}
		};
	};

	auto DoVoid()
	{
		return Awaitable<false>{};
	}

	VoidTask DoVoidTask()
	{
		co_await DoVoid();
		std::println("Ha");
	}

	void Run()
	{
		engn.seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
		Thread t1{};
		Thread t2{};
		t1.Start();
		t2.Start();

		//auto m = DoVoidTask();

		Task x = RunOnNewThread();
		Task y = RunOnNewThread();
		Sleep(2000);
		std::println("The x value is {}", x.Get());
		std::println("The y value is {}", y.Get());

		std::println("Finishing...");
		t1.SignalToExit();
		t2.SignalToExit();
		std::println("Bye...");
	}
}