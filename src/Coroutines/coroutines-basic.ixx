export module coroutines:basic;
import std;
import win32;

// https://github.com/Atliac/Coroutine-Tutorial/blob/master/hello_coroutine/main.cpp
//template<>
//struct std::coroutine_traits<void>
//{
//    struct promise_type
//    {
//        void get_return_object() noexcept {}
//        auto initial_suspend() noexcept { return std::suspend_never{}; }
//        auto final_suspend() noexcept { return std::suspend_never{}; }
//        void return_void() noexcept {}
//        void unhandled_exception() noexcept {}
//    };
//};
//
//export namespace Simple2
//{
//    void hello_coroutine()
//    {
//        std::println("Hello coroutine");
//        co_return ;
//    }
//
//    void Run()
//    {
//        hello_coroutine();
//    }
//}

export namespace Coroutines::Simplest
{
	struct resumable
	{
		struct awaitable
		{
			awaitable(resumable& t) : m_task(t)
			{

			}

			~awaitable()
			{
				std::println("~awaitable()");
			}

			bool await_ready()
			{
				std::println("await_ready()");
				return false;
			}

			void await_suspend(std::coroutine_handle<> h)
			{
				std::println("await_suspend()");
				//m_task.handle = h;
			}

			void await_resume()
			{
				std::println("await_resume()");
			}

			resumable& m_task;
		};

		awaitable operator co_await()  noexcept
		{
			return awaitable{ *this };
		}

		//std::coroutine_handle<> handle;
	};

	struct task
	{
		struct promise_type
		{
			~promise_type()
			{
				std::println("~promise_type()");
			}

			task get_return_object()
			{
				std::println("get_return_object()");
				return task(std::coroutine_handle<promise_type>::from_promise(*this));
			}

			std::suspend_never initial_suspend()
			{
				std::println("initial_suspend()");
				return {};
			}

			std::suspend_never final_suspend() noexcept
			{
				std::println("final_suspend()");
				return {};
			}

			void return_void()
			{
			}

			/*void return_value(int t)
			{
				std::println("return_value()");
			}*/

			void unhandled_exception()
			{
				std::println("unhandled_exception()");
			}
		};

		~task()
		{
			std::println("~task()");
		}

		task(std::coroutine_handle<promise_type> h) : m_h(h) {}

		void Get()
		{
			m_h.resume();
		}

		std::coroutine_handle<promise_type> m_h;
	};

	/*auto CreateAwaitable()
	{
		return awaitable{};
	}*/

	task MakeTask()
	{
		resumable r;
		std::println("MakeTask()");
		co_await r;
		// awaiter destroyed here
		//co_return 1;
		std::println("Hello");
	}

	void Run()
	{
		auto x = MakeTask();
		x.Get();
	}
}

export namespace Coroutines::EvenMoreBasicAwaitables
{

	template<typename T>
	struct OverlappedOperation
	{
		struct Awaitable
		{
			Awaitable() noexcept

			{
			}

			~Awaitable()noexcept
			{
				std::println("~awaitable()");
			}

			bool await_ready()noexcept // --> 2
			{
				std::println("await_ready()");
				return false;
			}

			void await_suspend(std::coroutine_handle<T> h) noexcept// --> 3
			{
				h.promise().set();
				std::println("await_suspend()");
				//H = h;
				//m_task.handle = h;
			}

			void await_resume()noexcept
			{
				std::println("await_result()");
			}
		};

		Awaitable operator co_await()  noexcept
		{
			//return Awaitable{ *this };
			return Awaitable{ };
		}
	};

	struct Task
	{
		struct promise_type
		{
			std::shared_ptr<int> M = std::make_shared<int>(0);

			void set() { *M = 10; }

			~promise_type()
			{
				std::println("promise_type::~promise_type()");
			}

			Task get_return_object()
			{
				return { std::coroutine_handle<promise_type>::from_promise(*this), M };
			}

			std::suspend_never initial_suspend() // --> 1
			{
				std::println("promise_type::initial_suspend()");
				return {  };
			}

			std::suspend_never final_suspend() noexcept
			{
				std::println("promise_type::final_suspend()");
				return {};
			}

			void return_void()
			{
			}

			void unhandled_exception()
			{
				std::println("promise_type::unhandled_exception()");
			}
		};

		std::coroutine_handle<promise_type> Handle;
		std::shared_ptr<int> M;

		Task(std::coroutine_handle<promise_type> h, std::shared_ptr<int> m)
			: Handle(h), M(m)
		{

		}

		~Task()
		{
			std::println("~task()");
		}

		void Continue()
		{
			Handle.resume();
		}
	};

	Task DoTask()
	{
		co_await OverlappedOperation<Task::promise_type>();
		std::println("resumed");
	}

	void Run()
	{
		std::println("Coroutines::EvenMoreBasicAwaitables");
		auto x = DoTask();
		x.Continue();
		std::println("{}", *x.M);
	}
}

// Adapted from Concurrency with Modern C++
export namespace Signal
{
	struct Event 
	{
		Event() = default;
		Event(const Event&) = delete;
		Event(Event&&) = delete;
		Event& operator=(const Event&) = delete;
		Event& operator=(Event&&) = delete;

		void notify() noexcept
		{
			std::println("Thread invoking notify() {}", std::this_thread::get_id());
			m_notified = true;

			void* waiter = m_suspendedWaiter.load();

			if (waiter != nullptr && m_suspendedWaiter.compare_exchange_strong(waiter, nullptr))
			{
				std::println("notify() compare_exchange_strong() {}", std::this_thread::get_id());
				static_cast<Awaiter*>(waiter)->coroutineHandle.resume();
			}
		}

		struct Awaiter
		{
			Awaiter(const Event& eve) : event(eve) {}

			bool await_ready() const
			{
				std::println("Awaited::await_ready() {}", std::this_thread::get_id());
				// allow at most one waiter
				if (event.m_suspendedWaiter.load() != nullptr)
				{
					throw std::runtime_error("More than one waiter is not valid");
				}

				// event.notified == false; suspends the coroutine
				// event.notified == true; the coroutine is executed like a normal function
				return event.m_notified;
			}
			bool await_suspend(std::coroutine_handle<> corHandle) noexcept
			{
				std::println("Awaited::await_suspend() {}", std::this_thread::get_id());
				coroutineHandle = corHandle;

				const Event& ev = event;
				ev.m_suspendedWaiter.store(this);

				if (ev.m_notified)
				{
					void* thisPtr = this;

					std::println("await_suspend() m_notified() {}", std::this_thread::get_id());
					if (ev.m_suspendedWaiter.compare_exchange_strong(thisPtr, nullptr))
					{
						std::println("await_suspend() compare_exchange_strong() {}", std::this_thread::get_id());
						return false;
					}
				}
				std::println("await_suspend() true {}", std::this_thread::get_id());
				return true;
			}

			void await_resume() noexcept 
			{
				std::println("Resuming thread {}", std::this_thread::get_id());
			}

		private:
			friend struct Event;
			const Event& event;
			std::coroutine_handle<> coroutineHandle;
		};

		Awaiter operator co_await() const noexcept
		{
			return Awaiter{ *this };
		}

	private:
		mutable std::atomic<void*> m_suspendedWaiter{ nullptr };
		mutable std::atomic<bool> m_notified{ false };
	};

	struct Task 
	{
		struct promise_type 
		{
			Task get_return_object() { return {}; }
			std::suspend_never initial_suspend() { return {}; }
			std::suspend_never final_suspend() noexcept { return {}; }
			void return_void() {}
			void unhandled_exception() {}
		};
	};

	Task receiver(Event & event) 
	{
		std::println("Receiver() {}", std::this_thread::get_id());
		auto start = std::chrono::high_resolution_clock::now();
		co_await event;
		std::println("Got the notification! {}", std::this_thread::get_id());
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		std::println("Waited {} seconds {}", elapsed.count(), std::this_thread::get_id());
	}

	void Run()
	{
		using namespace std::chrono_literals;

		std::println("Notification before waiting {}", std::this_thread::get_id());
		Event event1{};
		auto senderThread1 = std::thread( // notifies and exits
			[&event1] 
			{ 
				std::println("Thread doing notification {}", std::this_thread::get_id());
				event1.notify();
				std::println("Exiting {}", std::this_thread::get_id());
			}
		); // Notification
		auto receiverThread1 = std::thread(receiver, std::ref(event1)); // runs the coroutine

		receiverThread1.join();
		senderThread1.join();

		std::println("Notification after 2 seconds waiting {}\n", std::this_thread::get_id());

		Event event2{};
		auto receiverThread2 = std::thread(receiver, std::ref(event2)); // suspends at the coroutine and exits before thread2
		auto senderThread2 = std::thread( // notifies then runs the coroutine suspended by thread1
			[&event2] 
			{
				std::println("Thread doing notification {}", std::this_thread::get_id());
				std::this_thread::sleep_for(2s);
				event2.notify(); // Notification
				std::println("Exiting {}", std::this_thread::get_id());
			});

		receiverThread2.join();
		senderThread2.join();
	}
}

export namespace Win32Event
{
	struct Event
	{
		~Event() 
		{ 
			Win32::CloseHandle(Handle); 
		}

		Win32::HANDLE Handle = Win32::CreateEventW(nullptr, false, false, nullptr);
		
		void Signal()
		{
			Win32::SetEvent(Handle);
		}
		
		void Wait()
		{
			Win32::WaitForSingleObject(Handle, Win32::Infinite);
		}
	};

	struct Result
	{
		Event Done;
		int Number = 0;
	};

	template<typename TPromise>
	struct Awaitable
	{
		~Awaitable() 
		{ 
			std::println("Awaitable destroyed"); 
		}
		
		std::jthread& out;
		
		Awaitable(std::jthread* param) : out(*param) {}

		bool await_ready() 
		{ 
			std::println("Awaitable::await_ready()");
			return false;
		}

		void await_suspend(std::coroutine_handle<TPromise> h)
		{
			std::println("Awaitable::await_suspend()");
			out = std::jthread(
				[h]
				{
					std::this_thread::sleep_for(std::chrono::seconds{ 2 });
					h.resume();
				}
			);
		}
		
		void await_resume()
		{
			std::println("Awaitable::await_resume(): {}", std::this_thread::get_id());
		}
	};

	struct Task
	{
		~Task()
		{
			std::println("Task destroyed {}", std::this_thread::get_id());
		}

		std::shared_ptr<Result> value;

		Task(std::shared_ptr<Result> p) : value(std::move(p)) {}

		int Get()
		{
			value->Done.Wait();
			return value->Number;
		}

		struct promise_type // name must be promise_type
		{
			~promise_type() 
			{ 
				std::println("promise_type destroyed."); 
			}

			std::shared_ptr<Result> ptr = std::make_shared<Result>();
			
			Task get_return_object()
			{
				// Can pass this back to Task. Don't pass the promise_type
				// instance back as it's destroyed by then.
				//std::coroutine_handle<promise_type>::from_promise(*this);
				std::println("promise_type::get_return_object().");
				return { ptr };
			}
			
			std::suspend_never initial_suspend()
			{ 
				std::println("promise_type::initial_suspend().");
				return {};
			}

			std::suspend_never final_suspend() noexcept 
			{ 
				std::println("promise_type::final_suspend().");
				return {};
			}

			void unhandled_exception() {}

			void return_value(int i)
			{
				std::println("promise_type::return_value().");
				ptr->Number = i;
				ptr->Done.Signal();
			}
		};

		Task(std::jthread* jthread) : out(jthread) {}

		Awaitable<promise_type> operator co_await()
		{
			return Awaitable<promise_type>{ out };
		} 
		std::jthread* out;
	};

	Task ResumeOnNewThread1()
	{
		std::println("Coroutine started on thread: {}", std::this_thread::get_id());
		std::jthread out;
		co_await Awaitable<Task::promise_type>{ &out };
		// awaiter destroyed here
		std::println("Coroutine resumed on thread: {}", std::this_thread::get_id());
		co_return 8;
	}
	
	Task ResumeOnNewThread2() // Probably not better than 1
	{
		std::println("Coroutine started on thread: {}", std::this_thread::get_id());
		std::jthread out;
		co_await Task{&out};
		// awaiter destroyed here
		std::println("Coroutine resumed on thread: {}", std::this_thread::get_id());
		co_return 8;
	}

	void Run()
	{
		auto t = 
			ResumeOnNewThread1();
			//ResumeOnNewThread2();
		std::println("Got {}", t.Get());
	}
}

export namespace ExceptionTest
{
	struct Event
	{
		~Event()
		{
			Win32::CloseHandle(Handle);
		}

		Win32::HANDLE Handle = Win32::CreateEventW(nullptr, false, false, nullptr);

		void Signal()
		{
			Win32::SetEvent(Handle);
		}

		void Wait()
		{
			Win32::WaitForSingleObject(Handle, Win32::Infinite);
		}
	};

	struct Result
	{
		Event Done;
		int Number = 0;
	};

	template<typename TPromise>
	struct Awaitable
	{
		~Awaitable()
		{
			std::println("Awaitable destroyed");
		}

		auto await_ready() -> bool
		{
			std::println("Awaitable::await_ready()");
			return false;
		}

		void await_suspend(std::coroutine_handle<TPromise> h)
		{
			std::println("Awaitable::await_suspend()");
			std::jthread(
				[h]
				{
					std::this_thread::sleep_for(std::chrono::seconds{ 2 });
					h.resume();
				}
			).detach();
		}

		void await_resume()
		{
			std::println("Awaitable::await_resume(): {}", std::this_thread::get_id());
		}
	};

	struct Task
	{
		~Task()
		{
			std::println("Task destroyed {}", std::this_thread::get_id());
		}

		std::shared_ptr<Result> value;

		Task(std::shared_ptr<Result> p) : value(std::move(p)) {}

		auto Get() -> int
		{
			value->Done.Wait();
			return value->Number;
		}

		struct promise_type // name must be promise_type
		{
			~promise_type()
			{
				std::println("promise_type destroyed.");
			}

			std::shared_ptr<Result> ptr = std::make_shared<Result>();

			auto get_return_object() -> Task
			{
				// Can pass this back to Task. Don't pass the promise_type
				// instance back as it's destroyed by then.
				//std::coroutine_handle<promise_type>::from_promise(*this);
				std::println("promise_type::get_return_object().");
				return { ptr };
			}

			auto initial_suspend() -> std::suspend_never
			{
				std::println("promise_type::initial_suspend().");
				return {};
			}

			auto final_suspend() noexcept -> std::suspend_never
			{
				std::println("promise_type::final_suspend().");
				return {};
			}

			void unhandled_exception() 
			{
				std::println("An exception occurred.");
				ptr->Done.Signal();
			}

			void return_value(int i)
			{
				std::println("promise_type::return_value().");
				ptr->Number = i;
				ptr->Done.Signal();
			}
		};
	};

	auto ResumeOnNewThread() -> Task
	{
		std::println("Coroutine started on thread: {}", std::this_thread::get_id());
		co_await Awaitable<Task::promise_type>{};
		throw std::exception("a");
		// awaiter destroyed here
		std::println("Coroutine resumed on thread: {}", std::this_thread::get_id());
		co_return 8;
	}

	void Run()
	{
		auto t = ResumeOnNewThread();
		std::println("Got {}", t.Get());
	}
}

export namespace MoreCoro
{
	using namespace std::chrono_literals;
	// --- Tiny awaitable to "sleep" without blocking the coroutine ---------------

	struct sleep_awaitable 
	{
		std::chrono::milliseconds dur;
		bool await_ready() const noexcept { return dur.count() == 0; }   // run-through?
		void await_resume() const noexcept {}                             // nothing to return
		// Schedule resume on a background thread after 'dur'
		void await_suspend(std::coroutine_handle<> h) const 
		{
			std::thread(
				[h, d = dur] 
				{
					std::this_thread::sleep_for(d);
					h.resume();                                               // continue pipeline()
				}
			).detach();
		}
	};

	// --- Minimal Task so main() can wait for completion -------------------------
	struct Task 
	{
		struct promise_type 
		{
			std::binary_semaphore done{ 0 };
			Task get_return_object() 
			{
				return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
			}
			
			std::suspend_never initial_suspend() noexcept 
			{ 
				return {}; 
			}  // start immediately
			
			std::suspend_always final_suspend() noexcept 
			{                 // signal on finish
				done.release();
				return {};
			}

			void return_void() {}
			
			void unhandled_exception() { std::terminate(); }
		};

		std::coroutine_handle<promise_type> h;

		void wait() 
		{ 
			h.promise().done.acquire(); 
			h.destroy(); 
		}
	};

	// --- The async pipeline, but written linearly --------------------------------
	Task pipeline() 
	{
		std::cout << "Downloading...\n";
		co_await sleep_awaitable{ 500ms };   // pause here, resume later
		std::cout << "Processing...\n";
		co_await sleep_awaitable{ 400ms };
		std::cout << "Saving...\n";
		co_await sleep_awaitable{ 300ms };
		std::cout << "Saved: processed(data)\n";
	}

	void Run() 
	{
		auto t = pipeline();  // starts and runs until first co_await
		t.wait();             // wait for coroutine to finish
	}
}