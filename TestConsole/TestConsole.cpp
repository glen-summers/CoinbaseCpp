// TestConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../CoinbaseApi/interface/Factory.h"

#include "ConsoleKeyListener.h"

#include <iostream>

class Callbacks final : public Coinbase::StreamCallbacks
{
	void OnSnapshot(const std::string& product) noexcept override
	{
		(void) product;
	}

	void OnError(const std::string& errorMessage) noexcept override
	{
		std::cerr << "Error: " << errorMessage;
	}
};

int main()
{
	const auto & factory = Coinbase::Factory::CreateSandbox();
	
	Callbacks cb;
	auto stream = factory.CreateStream(cb);
	//auto stream = factory.CreateStreamWithProxy("127.0.0.1", 8888, cb); // fiddler

	ConsoleKeyListener ckl([&]()
	{
		std::cout << "stopping..." << std::endl;
		stream->Stop();
	});

	stream->Start();

	std::cout << "stopped" << std::endl;

	return 0;
}

