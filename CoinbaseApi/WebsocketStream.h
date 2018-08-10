#pragma once

#include "interface/stream.h"

class WebsocketStream final : public Coinbase::Stream
{
protected:// not needed as private class, also barfs with make_unique
	~WebsocketStream() = default;
};
