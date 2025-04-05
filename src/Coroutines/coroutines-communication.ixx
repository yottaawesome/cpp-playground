export module coroutines:communication;
import std;

export namespace Coroutines::Communication
{
	struct Result
	{
		bool Done = false;
		bool Cancel = false;
		int Number = 0;
	};

	struct Task
	{
		std::shared_ptr<Result> FinalResult;

		Task(std::shared_ptr<Result> result) 
			: FinalResult{ result } 
		{}

		struct PromiseType
		{
			std::shared_ptr<Result> ptr = std::make_shared<Result>();

			Task get_return_object()
			{
				return Task{ ptr };
			}

			std::suspend_never initial_suspend()
			{
				return {};
			}

			std::suspend_never final_suspend() noexcept
			{
				return {};
			}

			void unhandled_exception()
			{
			}

			// Called with the return value of the coroutine
			// if a return value is given.
			void return_value(int i)
			{
				ptr->Number = i;
				ptr->Done = true;
			}
		};

		using promise_type = PromiseType;

		struct Awaitable
		{
			std::jthread* Thread = nullptr;
			std::coroutine_handle<PromiseType> Handle;
			Awaitable(std::jthread* ptr)
				: Thread{ ptr }
			{
				if (not ptr)
					throw std::runtime_error("Invalid argument");
			}

			bool await_ready()
			{
				return false;
			}

			void await_suspend(std::coroutine_handle<PromiseType> h)
			{
				Handle = h;
				*Thread = std::jthread(
					[h]{
						h.resume();
					});
			}

			void await_resume()
			{
			}

			bool Cancelled()
			{
				return Handle.promise().ptr->Cancel;
			}
		};

		void Cancel()
		{
			FinalResult->Cancel = true;
		}

		bool Done()
		{
			return FinalResult->Done;
		}
	};

	Task Switch(std::jthread& thread)
	{
		Task::Awaitable awaitable{ &thread };
		co_await awaitable;

		while (not awaitable.Cancelled())
			std::this_thread::sleep_for(std::chrono::seconds{ 1 });

		co_return 1;
	}

	void Run()
	{
		std::jthread thread;
		Task t = Switch(thread);
		t.Cancel();
		while (not t.Done())
			std::this_thread::sleep_for(std::chrono::seconds{ 1 });
		std::println("Final result was {}", t.FinalResult->Number);
	}
}

export namespace Coroutines::Communication2
{
	struct Coawaitable
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

		Awaitable operator co_await()
		{
			return Awaitable{};
		}
	};

	struct Task
	{
		struct promise_type
		{
			Task get_return_object()
			{
				return {};
			}

			// coroutines.
			std::suspend_never initial_suspend()
			{
				return {};
			}

			std::suspend_never final_suspend() noexcept
			{
				return {};
			}

			// Called when an exception occurs.
			void unhandled_exception()
			{
			}

			// Called with the return value of the coroutine
			// if no return value is given.
			void return_void()
			{
			}
		};
	};

	Task Do()
	{
		co_await Coawaitable{};
	}

	void Run()
	{
	}
}

export namespace Coroutines::Communication3
{
	struct Awaitable
	{
		Awaitable() noexcept {}

		bool await_ready() noexcept
		{
			return false;
		}

		void await_suspend(auto h) noexcept
		{
		}

		void await_resume() noexcept
		{
		}
	};

	struct Task
	{
		struct promise_type
		{
			Task get_return_object()
			{
				return { std::coroutine_handle<promise_type>::from_promise(*this) };
			}

			auto initial_suspend()
			{
				return std::suspend_never{};
			}

			auto final_suspend() noexcept
			{
				return std::suspend_never{};
			}

			void return_value(int x)
			{
			}

			void unhandled_exception()
			{
			}
		};

		Task(std::coroutine_handle<promise_type> p) : Handle(p)
		{
		}

		std::coroutine_handle<promise_type> Handle;

		void Resume()
		{
			Handle.resume();
		}
	};

	Task Switch()
	{
		Awaitable a{};
		co_await a;
		std::println("Hello");
		co_return 1;
	}

	void Run()
	{
		Task t = Switch();
		t.Resume();
	}
}
