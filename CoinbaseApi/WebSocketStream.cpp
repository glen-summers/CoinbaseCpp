#include "stdafx.h"

#include "Heartbeat.h"
#include "WebsocketStream.h"

#include <boost/asio/connect.hpp>

namespace ssl = boost::asio::ssl;
namespace http = boost::beast::http;

namespace
{
	boost::asio::io_context ioContext;
	ssl::context sslContext { ssl::context::sslv23_client };
}

WebsocketStream::FunctionMap WebsocketStream::functionMap =
{
	{ "subscriptions", &WebsocketStream::ProcessSubscriptions },
	{ "snapshot", &WebsocketStream::ProcessSnapshot },
	{ "l2update", &WebsocketStream::ProcessUpdate },
	{ "heartbeat", &WebsocketStream::ProcessHeartbeat },
	{ "ticker", &WebsocketStream::ProcessTicker },
	{ "error", &WebsocketStream::ProcessError },
};

WebsocketStream::WebsocketStream(const std::string & host, unsigned short port, const std::string & proxyHost, unsigned short proxyPort,
	Coinbase::StreamCallbacks & callback)
	: host(host)
	, port(port)
	, proxyHost(proxyHost)
	, proxyPort(proxyPort)
	, resolver(ioContext)
	, webSocket(ioContext, sslContext)
	, callback(callback)
{
	std::cout << "WebsocketStream with proxy" << std::endl;
}

WebsocketStream::~WebsocketStream()
{
	std::cout << "~WebsocketStream" << std::endl;
}

void WebsocketStream::Start()
{
	std::cout << "Start" << std::endl;
	if (proxyHost.empty())
	{
		Connect();
	}
	else
	{
		ConnectProxy();
	}

	// run sync here, move to where?
	ioContext.run();
	//boost::asio::dispatch();
}

void WebsocketStream::Connect()
{
	auto self(shared_from_this());
	resolver.async_resolve(host, std::to_string(port), [this, self](boost::system::error_code ec, tcp::resolver::results_type results)
	{
		if (ec) return Fail(ec, "resolve");
		std::cout << "resolve" << std::endl;

		async_connect(webSocket.next_layer().next_layer(), results.begin(), results.end(), [this, self](boost::system::error_code ec, tcp::resolver::iterator i)
		{
			if (ec) return Fail(ec, "connect");
			std::cout << "connect" << std::endl;

			CompleteHandshake();
		});
	});
}

void WebsocketStream::ConnectProxy()
{
	auto self(shared_from_this());
	resolver.async_resolve(proxyHost, std::to_string(proxyPort), [this, self](boost::system::error_code ec, tcp::resolver::results_type results)
	{
		if (ec) return Fail(ec, "resolve proxy");
		std::cout << "resolve proxy" << std::endl;

		async_connect(webSocket.next_layer().next_layer(), results.begin(), results.end(), [this, self](boost::system::error_code ec, tcp::resolver::iterator i)
		{
			if (ec) return Fail(ec, "connect");
			std::cout << "connect" << std::endl;

			std::ostringstream target;
			target << host << ":" << port;
			auto req = std::make_shared<http::request<http::string_body>>(http::verb::connect, target.str(), 11);
			req->set(http::field::host, target.str());

			async_write(webSocket.next_layer().next_layer(), *req, [this, self, req](boost::system::error_code ec, std::size_t bytes_transferred)
			{
				UNREFERENCED_PARAMETER(bytes_transferred);
				if (ec) return Fail(ec, "proxyConnect");

				auto p = std::make_shared<http::parser<false, http::empty_body>>(http::response<http::empty_body>());
				p->skip(true);

				// read tcp connect response
				async_read(webSocket.next_layer().next_layer(), buffer, *p, [this, self, p](boost::system::error_code ec, std::size_t bytes_transferred)
				{
					UNREFERENCED_PARAMETER(bytes_transferred);
					if (ec) return Fail(ec, "proxyRead");
					std::cout << p->get() << std::endl;
					CompleteHandshake();
				});
			});
		});
	});
}

void WebsocketStream::CompleteHandshake()
{
	auto self(shared_from_this());
	webSocket.next_layer().async_handshake(ssl::stream_base::client, [this, self](boost::system::error_code ec)
	{
		if (ec) return Fail(ec, "ssl_handshake");
		std::cout << "ssl handshake" << std::endl;

		webSocket.async_handshake(host, "/", [this, self](boost::system::error_code ec)
		{
			if (ec) return Fail(ec, "handshake");
			std::cout << "handshake" << std::endl;

			// without heartbeat get stream_truncated error on sandbox endpoint, something to do with inactivity
			// not seen on live endpoint but unlikly to ever have inactivity, try just an user order update subscription with no orders on live?
			std::string subscription = R"({
    "type": "subscribe",
    "product_ids": ["BTC-EUR"],
    "channels": [ "level2", "heartbeat", "ticker" ]
})";

			webSocket.async_write(boost::asio::buffer(subscription), [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
			{
				UNREFERENCED_PARAMETER(bytes_transferred);
				if (ec) return Fail(ec, "write");
				std::cout << "write" << std::endl;
				Read();
			});
		});
	});
}

void WebsocketStream::Stop()
{
	// need to cancel async read?
	std::cout << "stop" << std::endl;
	ioContext.stop();
}

void WebsocketStream::Read()
{
	auto self(shared_from_this());
	webSocket.async_read(buffer, [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
	{
		UNREFERENCED_PARAMETER(bytes_transferred);
		if (ec) return Fail(ec, "read");
		std::ostringstream stm;
		stm << buffers(buffer.data());

		JsonDocument document;
		document.Parse(stm.str().c_str());

		auto type = document["type"].GetString();
		auto it = functionMap.find(type);
		if (it != functionMap.end())
		{
			//std::invoke(it->second, *this, document);
			(this->*(it->second))(document);
		}
		else
		{
			std::cout << "Unprocessed message: " << type << std::endl;
		}

		buffer = boost::beast::multi_buffer {};
		Read();
	});
}

void WebsocketStream::Fail(boost::system::error_code ec, char const* what)
{
	std::ostringstream stm;
	stm << what << " : " << ec.message() << " : " << ec.category().name();
	callback.OnError(stm.str());
}

void WebsocketStream::ProcessSubscriptions(const JsonDocument & document)
{
	UNREFERENCED_PARAMETER(document);
	std::cout << "sub response" << std::endl;
}

void WebsocketStream::ProcessError(const JsonDocument & document)
{
	// error from malformed json or server error
	// will be on an io thread, so cannot throw, notify on interface, receiver should resubscribe
	// get some "unable to establish level 2 for ..." but other channels are ticking
	// should treat all errors as fatal and require a resubscribe
	// log error here
	callback.OnError(document["message"].GetString());
}

void WebsocketStream::ProcessSnapshot(const JsonDocument & document)
{
	UNREFERENCED_PARAMETER(document);
	std::cout << "snap" << std::endl;
}

void WebsocketStream::ProcessUpdate(const JsonDocument & document)
{
	UNREFERENCED_PARAMETER(document);
	std::cout << "update" << std::endl;
}

void WebsocketStream::ProcessHeartbeat(const JsonDocument & document)
{
	auto hb = Heartbeat::Parse(document);

	std::cout << "hb: id=" << hb.Id() << ", Seq=" << hb.Seq() << std::endl;
}

void WebsocketStream::ProcessTicker(const JsonDocument & document)
{
	UNREFERENCED_PARAMETER(document);
	std::cout << "tick" << std::endl;
}
