#pragma once

#include "interface/stream.h"

#include <boost/system/config.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <string>

class WebsocketStream final : public Coinbase::Stream
{
	std::string const url;

public:
	WebsocketStream(const std::string & url);
	~WebsocketStream() = default;
};
