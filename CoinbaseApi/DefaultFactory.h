#pragma once

#include "interface\Factory.h"

#include <string>

namespace Coinbase
{
	class DefaultFactory final : public Factory
	{
		std::string const streamUrl;
		std::string const requestUrl;

	public:
		DefaultFactory(const std::string & streamUrl, const std::string & requestUrl);

	private:
		StreamPtr CreateStream(StreamCallbacks &) const override;
	};
}

