#pragma once

#include "interface\Factory.h"

#include <string>

namespace Coinbase
{
	class DefaultFactory final : public Factory
	{
		std::string const streamHost;
		short const streamPort;
		std::string const requestHost;
		short const requestPort;

	public:
		DefaultFactory(const std::string& streamHost, short streamPort, const std::string& requestHost, short requestPort);

	private:
		StreamPtr CreateStream(StreamCallbacks &) const override;
		StreamPtr CreateStreamWithProxy(const std::string& proxyServer, short proxyPort, StreamCallbacks&) const override;
	};
}

