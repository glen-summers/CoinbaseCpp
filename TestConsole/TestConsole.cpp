// TestConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../CoinbaseApi/interface/Factory.h"

class Callbacks final : public Coinbase::StreamCallbacks
{
	void OnSnapshot(const std::string& product) override
	{}
};

int main()
{
	const auto & factory = Coinbase::Factory::Create();
	
	Callbacks cb;
	auto stream = factory.CreateStream(cb);
	return 0;
}

