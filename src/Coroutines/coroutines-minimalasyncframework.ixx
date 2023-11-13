// A port of Jeremy Ong's code located here https://www.jeremyong.com/cpp/2021/01/04/cpp20-coroutines-a-minimal-async-framework/
// with various modifications to fix errors and make it compile.
// See also the author's coroutine lib: https://github.com/jeremyong/coop
module;

#include <Windows.h>

export module coroutines:minimalasyncframework;
import std;

export namespace MinimalAsyncFrameWork
{
	// just a hack to work with std::format
	auto get_thread_id()
	{
		return GetCurrentThreadId();
	}

	class CTP 
	{
		public:
			// The constructor should spawn thread_count threads and begin the run loop of each thread
			CTP(int thread_count)
			{ 
				// todo
			}

			// For simplicity, we'll assume a global singleton instance is available
			static CTP& instance()
			{
				static CTP ctp(5);
				return ctp;
			}

			// When a coroutine is enqueued, the coroutine is passed to one of the CTP threads, and
			// eventually coroutine.resume() should be invoked when the thread is ready. You can
			// implement work stealing here to "steal" coroutines on busy threads from idle ones, or
			// any other load balancing/scheduling scheme of your choice.
			//
			// NOTE: This should (obviously) be threadsafe
			void enqueue(std::coroutine_handle<> coroutine)
			{
				// todo
			}
	};

	struct event_awaiter 
	{
		HANDLE event;

		event_awaiter(HANDLE h) : event(h) {}
		bool await_ready() const noexcept 
		{
			// Check if we need to bother suspending at all by seeing if the
			// event was already signaled with a non-blocking wait.
			return WaitForSingleObject(event, 0) == WAIT_OBJECT_0;
		}

		void await_suspend(std::coroutine_handle<> coroutine) const noexcept 
		{
			// The coroutine handle passed here can be copied elsewhere and resumed
			// when the event is signaled. Here, we spawn a dedicated thread for
			// demonstration purposes, but you should have a dedicated low-priority
			// thread to queue waiting events to.
			std::thread thread{ 
				[coroutine, event=event]() noexcept 
				{
					WaitForSingleObject(event, INFINITE);
					// The CTP will call coroutine.resume() on an available thread now
					CTP::instance().enqueue(coroutine);
				}
			};
			thread.detach();
		}

		void await_resume() noexcept 
		{
			// This is called after the coroutine is resumed in the async thread
			std::cout << std::format("Event signaled, resuming on thread {}\n", get_thread_id());
		}
	};

	inline auto suspend() noexcept {
		struct awaiter {
			// Unlike the OS event case, there's no case where we suspend and the work
			// is immediately ready
			bool await_ready() const noexcept { return false; }

			// Since await_ready() always returns false, when suspend is called, we will
			// always immediately suspend and call this function (which enqueues the coroutine
			// for immediate reactivation on a different thread)
			void await_suspend(std::coroutine_handle<> coroutine) noexcept {
				CTP::instance().enqueue(coroutine);
			}

			void await_resume() const noexcept {
				std::cout << std::format("Suspended task now running on thread {}\n", get_thread_id());
			}
		};
		return awaiter{};
	}

	template <typename T>
	struct task {
		// The return type of a coroutine must contain a nested struct or type alias called `promise_type`
		struct promise_type 
		{
			// Keep a coroutine handle referring to the parent coroutine if any. That is, if we
			// co_await a coroutine within another coroutine, this handle will be used to continue
			// working from where we left off.
			std::coroutine_handle<> precursor;

			// Place to hold the results produced by the coroutine
			T data;

			// Invoked when we first enter a coroutine. We initialize the precursor handle
			// with a resume point from where the task is ultimately suspended
			task get_return_object() noexcept
			{
				return { std::coroutine_handle<promise_type>::from_promise(*this) };
			}

			// When the caller enters the coroutine, we have the option to suspend immediately.
			// Let's choose not to do that here
			std::suspend_never initial_suspend() const noexcept { return {}; }

			// If an exception was thrown in the coroutine body, we would handle it here
			void unhandled_exception() {}

			// The coroutine is about to complete (via co_return or reaching the end of the coroutine body).
			// The awaiter returned here defines what happens next
			auto final_suspend() const noexcept 
			{
				struct awaiter 
				{
					// Return false here to return control to the thread's event loop. Remember that we're
					// running on some async thread at this point.
					bool await_ready() const noexcept { return false; }

					void await_resume() const noexcept {}

					// Returning a coroutine handle here resumes the coroutine it refers to (needed for
					// continuation handling). If we wanted, we could instead enqueue that coroutine handle
					// instead of immediately resuming it by enqueuing it and returning void.
					std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept {
						auto precursor = h.promise().precursor;
						if (precursor)
						{
							return precursor;
						}
						return std::noop_coroutine();
					}
				};
				return awaiter{};
			}

			// When the coroutine co_returns a value, this method is used to publish the result
			void return_value(T value) noexcept
			{
				data = std::move(value);
			}
		};

		// The following methods make our task type conform to the awaitable concept, so we can
		// co_await for a task to complete

		bool await_ready() const noexcept {
			// No need to suspend if this task has no outstanding work
			return handle.done();
		}

		T await_resume() const noexcept {
			// The returned value here is what `co_await our_task` evaluates to
			return std::move(handle.promise().data);
		}

		void await_suspend(std::coroutine_handle<> coroutine) const noexcept {
			// The coroutine itself is being suspended (async work can beget other async work)
			// Record the argument as the continuation point when this is resumed later. See
			// the final_suspend awaiter on the promise_type above for where this gets used
			handle.promise().precursor = coroutine;
		}

		// This handle is assigned to when the coroutine itself is suspended (see await_suspend above)
		std::coroutine_handle<promise_type> handle;
	};


	int some_expensive_thing() { return 1; }

	void do_something_else() {}

	// Note, we do not yet know what a "task" is. That will be introduced next
	task<int> async_routine() {
		// Suspend this current coroutine and enqueue it to our CTP
		co_await suspend();

		// At this point, we're running on a thread that resumed this coroutine's handle
		// In this case, we compute some integer and "co_return" it
		co_return some_expensive_thing();
	}

	task<int> another_async_routine() {
		// Enqueue 3 invocations of async_routine such that they are all running
		// in parallel, assuming we have 3 threads available
		// The current thread enters `async_routine`, then encounters the co_await of the
		// suspend() above. This yields control back so we can spawn the next async_routine.
		// In each case, get_return_object is called to construct a task with memory to
		// store the returned fresult. Furthermore, the coroutine handle to each of these
		// invocations is stored so that when async_routine suspends internally, this
		// caller can continue where it left off.
		task<int> t1 = async_routine();
		task<int> t2 = async_routine();
		task<int> t3 = async_routine();

		// Because t2 is itself an awaitable, a few things happen here. First, we check
		// in await_ready above if the coroutine handle associated with t2 has already
		// finalized. If so, we don't need to suspend and can just retrieve the result
		// directly (internally invokes `return_value`). If t2 hasn't finished yet, we
		// assign the precursor handle to the current resume point and suspend. When t2
		// finishes later, its final_suspend will find this precursor and resume here.
		// Note that it's possible that after this line, we end up running on a different
		// thread (the thread that finished t2)
		int r2 = co_await t2;

		do_something_else();

		// Resume points work in the middle of expressions as well
		co_return ((co_await t1) + r2 + (co_await t3));
	}
}
