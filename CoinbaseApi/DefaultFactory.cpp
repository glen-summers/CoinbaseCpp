#include "stdafx.h"

#include "DefaultFactory.h"
#include "WebsocketStream.h"

using Coinbase::DefaultFactory;

namespace
{
	const char StreamHost[] = "ws-feed.pro.coinbase.com";
	const char RequestHost[] = "api.pro.coinbase.com";

	const char SandboxStreamHost[] = "ws-feed-public.sandbox.pro.coinbase.com";
	const char SandboxRequestHost[] = "api-public.sandbox.pro.coinbase.com";

	DefaultFactory liveFactory(StreamHost, RequestHost);
	DefaultFactory sandboxFactory(SandboxStreamHost, SandboxRequestHost);
}

const Coinbase::Factory& Coinbase::Factory::Create()
{
	return liveFactory;
}

const Coinbase::Factory& Coinbase::Factory::CreateSandbox()
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
	return std::make_shared<WebsocketStream>(streamUrl);
}
