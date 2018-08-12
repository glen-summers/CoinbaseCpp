#include "stdafx.h"

#include "WebsocketStream.h"

#include <boost/asio/connect.hpp>

namespace ssl = boost::asio::ssl;
namespace http = boost::beast::http;

namespace
{
	boost::asio::io_context ioContext;
	ssl::context sslContext { ssl::context::sslv23_client };
}

WebsocketStream::WebsocketStream(const std::string & host, short port)
	: WebsocketStream(host, port, {}, {})
{}

WebsocketStream::WebsocketStream(const std::string & host, short port, const std::string & proxyHost, short proxyPort)
	: host(host)
	, port(port)
	, proxyHost(proxyHost)
	, proxyPort(proxyPort)
	, resolver(ioContext)
	, webSocket(ioContext, sslContext)
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

			std::string subscription = R"({
    "type": "subscribe",
    "product_ids": [
        "BTC-EUR"
    ],
    "channels": [
        "level2"
    ]
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
		std::cout << "read " << bytes_transferred << " : " << buffers(buffer.data()) << std::endl;
	 	if (ec) return Fail(ec, "read");

		buffer = boost::beast::multi_buffer {};
		Read(); // how stop read loop?
	});
}

void WebsocketStream::Fail(boost::system::error_code ec, char const* what)
{
	std::cerr << what
		<< " : " << ec.message()
		<< " : " << ec.category().name() << "\n";
	// call error callback
}
