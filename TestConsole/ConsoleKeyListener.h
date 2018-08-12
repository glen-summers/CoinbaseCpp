#pragma once

#include <future>
#include <iostream>

class ConsoleKeyListener
{
	std::future<void> f;
	std::function<void()> stop;
public:
	ConsoleKeyListener(const std::function<void()> & stop)
		: stop(stop)
	{
		f = std::async(std::launch::async, [this]
		{
			std::cout << "press key to exit" << std::endl;
			std::getchar();
			this->stop();
		});
	}
};

