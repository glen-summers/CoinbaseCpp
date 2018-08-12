// TestConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../CoinbaseApi/interface/Factory.h"

#include "ConsoleKeyListener.h"

#include <iostream>

class Callbacks final : public Coinbase::StreamCallbacks
{
	void OnSnapshot(const std::string& product) override
	{
		(void) product;
	}
};

int main()
{
	const auto & factory = Coinbase::Factory::CreateSandbox();
	
	Callbacks cb;
	auto stream = factory.CreateStream(cb);

	ConsoleKeyListener ckl([&]()
	{
		std::cout << "stopping..." << std::endl;
		stream->Stop();
	});

	stream->Start();

	std::cout << "stopped" << std::endl;

	return 0;
}

