export module coroutines:pool;
import std;

export namespace Coroutines::Pool
{
	struct WorkerThread
	{
		~WorkerThread()
		{
			/*if (m_coroutine.done())
				m_coroutine.destroy();*/
		}

		WorkerThread() = default;

		void Start()
		{
			m_thread = std::jthread(&WorkerThread::Run, this);
		}

		void Run()
		{
			while (m_keepRunning)
			{
				m_hasWork.acquire();
				if (not m_keepRunning)
					return;
				std::println("Resuming on {}.", std::this_thread::get_id());
				m_coroutine.resume();
			}
		}

		void Add(std::coroutine_handle<> coro)
		{
			m_coroutine = coro;
			m_hasWork.release();
		}

		void SignalToExit()
		{
			m_keepRunning = false;
			m_hasWork.release();
		}

		std::jthread m_thread;
		std::binary_semaphore m_hasWork{ 0 };
		std::atomic<bool> m_keepRunning = true;

		std::coroutine_handle<> m_coroutine;
	};

	struct WorkerThreadPool
	{
		~WorkerThreadPool()
		{
			Exit();
		}

		WorkerThreadPool()
		{
			std::ranges::for_each(m_thread, [](auto& t) {t.Start(); });
		}
		std::array<WorkerThread, 3> m_thread;
		int current = 0;

		void Schedule(std::coroutine_handle<> h)
		{
			m_thread[current%m_thread.size()].Add(h);
			current++;
		}

		void Exit()
		{
			std::ranges::for_each(m_thread, [](auto& t) {t.SignalToExit(); });
		}
	};

	WorkerThreadPool Pool;

	struct Awaitable
	{
		bool await_ready()
		{
			return false;
		}

		void await_suspend(std::coroutine_handle<> h)
		{
			Pool.Schedule(h);
		}

		void await_resume()
		{
		}
	};

	template<typename T>
	struct Task
	{
		std::shared_ptr<T> value;

		Task(std::shared_ptr<T> t) :value{ t } {}

		struct promise_type
		{
			std::shared_ptr<T> inner_value = std::make_shared<T>();
			Task get_return_object() noexcept { return Task<T>{ inner_value }; }
			std::suspend_never initial_suspend() noexcept { return {}; }
			std::suspend_never final_suspend() noexcept { return {}; }
			void unhandled_exception() noexcept {}

			void return_value(T v) noexcept
			{
				*inner_value = v;
			}
		};
	};

	template<>
	struct Task<void>
	{
		struct promise_type
		{
			Task get_return_object() noexcept { return{}; }
			std::suspend_never initial_suspend() noexcept { return {}; }
			std::suspend_never final_suspend() noexcept { return {}; }
			void unhandled_exception() noexcept {}
			void return_void() noexcept { }
		};
	};


	template<std::invocable T>
	Task<std::invoke_result_t<T>> Do(T&& fn)
	{
		co_await Awaitable{};
		co_return std::invoke(fn);
	}

	void Run()
	{
		Do([] { std::println("Hello A!"); });
		Do([] { std::println("Hello B!"); });
		Do([] { std::println("Hello C!"); });
		std::this_thread::sleep_for(std::chrono::seconds{ 2 });
		Pool.Exit();

		//std::println("{}", *x.value);

	}
}

export namespace Coroutines::Pool2
{
	struct Awaitable
	{
		bool await_ready()
		{
			return false;
		}

		void await_suspend(std::coroutine_handle<> h)
		{
		}

		void await_resume()
		{
		}
	};

	template<typename T>
	struct Task
	{
		std::shared_ptr<T> value;

		struct promise_type
		{
			std::shared_ptr<T> inner_value = std::make_shared<T>();
			Task get_return_object() noexcept { return Task<T>{ inner_value, std::coroutine_handle<promise_type>::from_promise(*this) }; }
			std::suspend_always initial_suspend() noexcept { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }
			void unhandled_exception() noexcept {}

			void return_value(T v) noexcept
			{
				*inner_value = v;
			}
		};

		void Continue()
		{
			coroutine.resume();
		}

		Task& Do(this Task& self)
		{
			return self;
		}

		std::coroutine_handle<promise_type> coroutine;
		Task(std::shared_ptr<T> t, std::coroutine_handle<promise_type> h) :value{ t }, coroutine{ h } {}
	};

	template<>
	struct Task<void>
	{
		struct promise_type
		{
			Task get_return_object() noexcept { return{ std::coroutine_handle<promise_type>::from_promise(*this) }; }
			std::suspend_always initial_suspend() noexcept { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }
			void unhandled_exception() noexcept {}
			void return_void() noexcept {}
		};

		std::coroutine_handle<promise_type> coroutine;
		Task(std::coroutine_handle<promise_type> h) : coroutine{ h } {}

		void Continue()
		{
			// can check the promise state before resuming
			//x.coroutine.promise().
			coroutine.resume();
		}
	};


	template<std::invocable T>
	Task<std::invoke_result_t<T>> Do(T&& fn)
	{
		//co_await Awaitable{};
		co_return std::invoke(fn);
	}

	void Run()
	{
		auto x = Do([] { std::println("Hello A!"); return 1; });
		auto y = Do([] { std::println("Hello B!"); });
		auto z = Do([] { std::println("Hello C!"); });
		std::this_thread::sleep_for(std::chrono::seconds{ 1 });

		x.Continue();
		y.Continue();
		z.Continue();
		std::println("{}", *x.value);
	}
}
