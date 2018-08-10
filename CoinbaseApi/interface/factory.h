#pragma once

#include "stream.h"
#include "streamcallbacks.h"

#include <memory>

namespace Coinbase
{
	typedef std::unique_ptr<Stream> StreamPtr;

	struct Factory
	{
		virtual StreamPtr CreateStream(StreamCallbacks &) const = 0;// , const Subscription &, Auth * auth = nullptr) const = 0;
		virtual ~Factory() = default;

		static const Factory& Create();
	};
}
