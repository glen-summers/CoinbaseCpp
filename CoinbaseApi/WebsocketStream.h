#pragma once

#include "interface/stream.h"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp> // omision causes Unknown Socket type in async_teardown.
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <string>

class WebsocketStream final
	: public std::enable_shared_from_this<WebsocketStream>
	, public Coinbase::Stream
{
	using tcp = boost::asio::ip::tcp;
	using WebSocket = boost::beast::websocket::stream<boost::asio::ssl::stream<tcp::socket>>;
	using Buffer = boost::beast::multi_buffer;

	std::string const host;
	short port;

	std::string const proxyHost;
	short proxyPort;

	tcp::resolver resolver;
	WebSocket webSocket;
	Buffer buffer;

public:
	WebsocketStream(const std::string & host, short port);
	WebsocketStream(const std::string & host, short port, const std::string & proxyHost, short proxyPort);
	~WebsocketStream(); // = default;

	void Start() override;
	void Stop() override;

private:
	void Connect();
	void ConnectProxy();
	void CompleteHandshake();
	void Read();
	void Fail(boost::system::error_code ec, char const* what);
};
