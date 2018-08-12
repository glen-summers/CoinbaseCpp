#include "stdafx.h"

#include "DefaultFactory.h"
#include "WebsocketStream.h"

using Coinbase::DefaultFactory;

namespace
{
	const char StreamHost[] = "ws-feed.pro.coinbase.com";
	const short StreamPort = 443;
	const char RequestHost[] = "api.pro.coinbase.com";
	const short RequestPort = 443;

	const char SandboxStreamHost[] = "ws-feed-public.sandbox.pro.coinbase.com";
	const short SandboxStreamPort = 443;
	const char SandboxRequestHost[] = "api-public.sandbox.pro.coinbase.com";
	const short SandboxRequestPort = 443;

	DefaultFactory liveFactory(StreamHost, StreamPort, RequestHost, RequestPort);
	DefaultFactory sandboxFactory(SandboxStreamHost, SandboxStreamPort, SandboxRequestHost, SandboxRequestPort);
}

const Coinbase::Factory& Coinbase::Factory::Create()
{
	return liveFactory;
}

const Coinbase::Factory& Coinbase::Factory::CreateSandbox()
{
	return sandboxFactory;
}

DefaultFactory::DefaultFactory(const std::string& streamHost, short streamPort, const std::string& requestHost, short requestPort)
	: streamHost(streamHost), streamPort(streamPort)
	, requestHost(requestHost), requestPort(requestPort)
{
}

Coinbase::StreamPtr DefaultFactory::CreateStream(Coinbase::StreamCallbacks&) const
{
	return std::make_shared<WebsocketStream>(streamHost, streamPort);
}

Coinbase::StreamPtr DefaultFactory::CreateStreamWithProxy(const std::string& proxyServer, short proxyPort, Coinbase::StreamCallbacks&) const
{
	return std::make_shared<WebsocketStream>(streamHost, streamPort, proxyServer, proxyPort);
}
