#include "stdafx.h"

#include "DefaultFactory.h"
#include "WebsocketStream.h"

using Coinbase::DefaultFactory;

namespace
{
	const char StreamUrl[] = "wss://ws-feed.pro.coinbase.com";
	const char RequestUrl[] = "https://api.pro.coinbase.com";

	const char SandboxStreamUrl[] = "wss://ws-feed-public.sandbox.gdax.com";
	const char SandboxRequestUrl[] = "https://api-public.sandbox.gdax.com";

	// not working yet..
	//const char SandboxStreamUrl[] = "wss://ws-feed-public.sandbox.pro.coinbase.com";
	//const char SandboxRequestUrl[] = "https://api-public.sandbox.pro.coinbase.com";

	//DefaultFactory liveFactory(StreamUrl, RequestUrl);
	DefaultFactory sandboxFactory(SandboxStreamUrl, SandboxRequestUrl);
}

const Coinbase::Factory& Coinbase::Factory::Create()
{
	return sandboxFactory;
}

DefaultFactory::DefaultFactory(const std::string& streamUrl, const std::string& requestUrl)
	: streamUrl(streamUrl)
	, requestUrl(requestUrl)
{
}

Coinbase::StreamPtr DefaultFactory::CreateStream(Coinbase::StreamCallbacks&) const
{
	//Stream s;
	//return (Coinbase::StreamPtr)std::make_unique<WebsocketStream>();
	return std::unique_ptr<Stream>(new WebsocketStream());
}
