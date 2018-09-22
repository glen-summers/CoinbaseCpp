#pragma once

#include "interface/stream.h"
#include "interface/streamcallbacks.h"

#include <GLib/flogging.h>

#include <rapidjson/document.h>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp> // omision causes Unknown Socket type in async_teardown.
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <string>
#include <unordered_map>

class WebsocketStream final
	: public std::enable_shared_from_this<WebsocketStream>
	, public Coinbase::Stream
{
	inline static const GLib::Flog::Log log = GLib::Flog::LogManager::GetLog<WebsocketStream>();

	using tcp = boost::asio::ip::tcp;
	using WebSocket = boost::beast::websocket::stream<boost::asio::ssl::stream<tcp::socket>>;
	using Buffer = boost::beast::multi_buffer;

	using JsonDocument = rapidjson::Document;

	typedef std::unordered_map<std::string, void(WebsocketStream::*)(const JsonDocument & document)> FunctionMap;
	static FunctionMap functionMap;

	Coinbase::StreamCallbacks & callback;

	std::string const host;
	unsigned short port;

	std::string const proxyHost;
	unsigned short proxyPort;

	tcp::resolver resolver;
	WebSocket webSocket;
	Buffer buffer;

public:
	WebsocketStream(const std::string & host, unsigned short port, const std::string & proxyHost, unsigned short proxyPort,
		Coinbase::StreamCallbacks & callback);

	void Start() override;
	void Stop() override;

private:
	void Connect();
	void ConnectProxy();
	void CompleteHandshake();
	void Read();
	void Fail(boost::system::error_code ec, char const* what);

	void ProcessSubscriptions(const JsonDocument & object);
	void ProcessError(const JsonDocument & object);
	void ProcessSnapshot(const JsonDocument & object);
	void ProcessUpdate(const JsonDocument & object);
	void ProcessHeartbeat(const JsonDocument & object);
	void ProcessTicker(const JsonDocument & object);
};
