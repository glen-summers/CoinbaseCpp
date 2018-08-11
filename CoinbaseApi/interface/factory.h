#pragma once

#include "stream.h"
#include "streamcallbacks.h"

#include <memory>

#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

namespace Coinbase
{
	typedef std::shared_ptr<Stream> StreamPtr;

	struct Factory
	{
		virtual StreamPtr CreateStream(StreamCallbacks &) const = 0;// , const Subscription &, Auth * auth = nullptr) const = 0;
		virtual ~Factory() = default;

		static const Factory& Create();
		static const Factory& CreateSandbox();
	};
}
