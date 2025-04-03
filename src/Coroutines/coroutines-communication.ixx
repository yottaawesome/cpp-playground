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

	template<typename TPromise>
	struct Awaitable
	{
		std::jthread* Thread = nullptr;
		std::coroutine_handle<TPromise> Handle;
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

		void await_suspend(std::coroutine_handle<TPromise> h)
		{
			Handle = h;
			*Thread = std::jthread([h] 
			{ 
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

	struct Task
	{
		std::shared_ptr<Result> FinalResult;

		Task(std::shared_ptr<Result> result) 
			: FinalResult{ result } 
		{}

		struct promise_type
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
		Awaitable<Task::promise_type> awaitable{ &thread };
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
	}
}