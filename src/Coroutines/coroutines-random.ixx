module;

#include <Windows.h>
#pragma comment(lib, "Synchronization.lib")

export module coroutines:random;
import std;

export namespace Coroutines::Random
{
	struct Queue
	{
		public:
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

	struct awaitable
	{
		bool await_ready() { return false; }
		void await_suspend(std::coroutine_handle<> h)
		{
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
		public:
			void Start()
			{
				m_thread = std::jthread{ Thread::EntryPoint, this };
			}

			void SignalToExit()
			{
				m_run = false;
			}

		private:
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

export namespace Coroutines::Random2
{
	struct Queue
	{
		public:
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

	struct awaitable
	{
		bool await_ready() { return false; }
		void await_suspend(std::coroutine_handle<> h)
		{
			WorkQueue.Add(h);
		}
		void await_resume()
		{
			std::println("AwaitResume(): {}", std::this_thread::get_id());
		}
	};

	struct task
	{
		std::shared_ptr<int> value;
		task(std::shared_ptr<int> p) : value(p) { }
		int compare = 0;

		int get()
		{
			while (*value == 0)
			{
				WaitOnAddress(
					&*value,
					&compare,
					sizeof(int),
					INFINITE
				);
			}

			return *value;
		}

		struct promise_type
		{
			std::shared_ptr<int> ptr = std::make_shared<int>();
			task get_return_object()
			{
				return { ptr };
			}
			std::suspend_never initial_suspend() { return {}; }
			std::suspend_never final_suspend() noexcept { return {}; }
			void unhandled_exception() {}
			void return_value(int i)
			{
				*ptr = i;
				WakeByAddressSingle(
					&*ptr
				);
			}
		};
	};

	struct Thread
	{
		public:
			void Start()
			{
				m_thread = std::jthread{ Thread::EntryPoint, this };
			}

			void SignalToExit()
			{
				m_run = false;
			}

		private:
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
		engn.seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
		Thread t1{};
		Thread t2{};
		t1.Start();
		t2.Start();

		task x = resuming_on_new_thread();
		task y = resuming_on_new_thread();
		Sleep(2000);
		std::println("The x value is {}", x.get());
		std::println("The y value is {}", y.get());

		std::println("Finishing...");
		t1.SignalToExit();
		t2.SignalToExit();
		std::println("Bye...");
	}
}