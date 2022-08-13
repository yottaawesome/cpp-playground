#include <iostream>
#include <functional>
#include <thread>
#include <vector>

//https://habr.com/en/post/665730/

import eventloop;

std::function<void(std::vector<char>&)> OnNetworkEvent;

void Basic()
{
	Demo::EventLoop eventLoop;
	eventLoop.enqueue(
		[] {
			std::cout << "message from a different thread\n";
		}
	);
}

void emitNetworkEvent(Demo::EventLoop& loop, std::vector<char> data)
{
	if (!OnNetworkEvent) 
		return;

	loop.enqueue(std::bind(std::ref(OnNetworkEvent), std::move(data)));
}

void Demo2()
{
	//registering event handler
	OnNetworkEvent = [](std::vector<char>& message)
	{
		std::cout << message.size() << ' ';
	};

	Demo::EventLoop loop;

	//let's trigger the event from different threads
	std::thread t1 = std::thread(
		[](Demo::EventLoop& loop)
		{
			for (std::size_t i = 0; i < 10; ++i)
			{
				emitNetworkEvent(loop, std::vector<char>(i));
			}
		}, 
		std::ref(loop)
	);

	std::thread t2 = std::thread(
		[](Demo::EventLoop& loop)
		{
			for (int i = 10; i < 20; ++i)
			{
				emitNetworkEvent(loop, std::vector<char>(i));
			}
		}, 
		std::ref(loop)
	);

	for (int i = 20; i < 30; ++i)
	{
		emitNetworkEvent(loop, std::vector<char>(i));
	}

	t1.join();
	t2.join();

	loop.enqueue([]{ std::cout << std::endl; });
}



int main(int argc, char* argv[])
{
	Demo2();

    return 0;
}
