# Coroutines notes

## Awaitable

```
struct Awaitable
{
	// Return true if the result is already available; false otherwise.
	bool await_ready() 
	{
		...
	}

	// Called when initially suspended. The coroutine handle can be
	// stored and used to resume the coroutine, possibly on another thread.
	void await_suspend(std::coroutine_handle<> h)
	{
		...
	}

	// Called when the coroutine is resumed.
	void await_resume()
	{
		...
	}
};
```

## Task

```C++
struct Task
{
	// Define members here to retrieve the value from promise_type.

	// Name must be promise_type or a using must be defined.
	struct promise_type
	{
		// Return an instance of the enclosing object.
		Task get_return_object()
		{
			...
		}

		// Return suspend_always or suspend_never for lazy or eager 
		// coroutines.
		std::suspend_never initial_suspend() 
		{ 
			...
		}
		
		// Return suspend_always or suspend_never for lazy or eager 
		// coroutines.
		std::suspend_never final_suspend() noexcept 
		{ 
			...
		}

		// Called when an exception occurs.
		void unhandled_exception() 
		{
			...
		}
		
		// Called with the return value of the coroutine
		// if a return value is given.
		void return_value(int i)
		{
			...
		}

		// Called with the return value of the coroutine
		// if no return value is given.
		void return_void()
		{
			...
		}
	};
};
```

## Additional resources

* [theshoemaker's Yet Another C++ Coroutine Tutorial](https://theshoemaker.de/posts/yet-another-cpp-coroutine-tutorial)