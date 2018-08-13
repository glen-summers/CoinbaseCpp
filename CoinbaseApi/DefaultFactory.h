#pragma once

#include "interface\Factory.h"

#include <string>

namespace Coinbase
{
	class DefaultFactory final : public Factory
	{
		std::string const streamHost;
		unsigned short const streamPort;
		std::string const requestHost;
		unsigned short const requestPort;

	public:
		DefaultFactory(const std::string& streamHost, unsigned short streamPort, const std::string& requestHost, unsigned short requestPort);

	private:
		StreamPtr CreateStream(StreamCallbacks &) const override;
		StreamPtr CreateStreamWithProxy(const std::string& proxyServer, unsigned short proxyPort, StreamCallbacks&) const override;
	};
}

