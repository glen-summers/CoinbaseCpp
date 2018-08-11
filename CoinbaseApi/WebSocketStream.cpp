#include "stdafx.h"

#include "WebsocketStream.h"

#include <boost/asio/connect.hpp>

namespace
{
	boost::asio::io_context ioContext;
	boost::asio::ssl::context sslContext { boost::asio::ssl::context::sslv23_client };
}

WebsocketStream::WebsocketStream(const std::string& url)
	: host(url)
	, resolver(ioContext)
	, webSocket(ioContext, sslContext)
{
	std::cout << "WebsocketStream" << std::endl;
}

void WebsocketStream::Start()
{
	std::cout << "Start" << std::endl;

	auto self(shared_from_this());
	resolver.async_resolve(host, port, [this, self](boost::system::error_code ec, tcp::resolver::results_type results)
	{
		if (ec) return Fail(ec, "resolve");
		std::cout << "resolve" << std::endl;

		async_connect(webSocket.next_layer().next_layer(), results.begin(), results.end(), [this, self](boost::system::error_code ec, tcp::resolver::iterator i)
		{
			if (ec) return Fail(ec, "connect");
			std::cout << "connect" << std::endl;

			webSocket.next_layer().async_handshake(boost::asio::ssl::stream_base::client, [this, self](boost::system::error_code ec)
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
		});
	});

	// run sync here, move to where?
	ioContext.run();
}

void WebsocketStream::Read()
{
	auto self(shared_from_this());
	webSocket.async_read(buffer, [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
	{
	 	UNREFERENCED_PARAMETER(bytes_transferred);
	 	if (ec) return Fail(ec, "read");

		std::cout << buffers(buffer.data()) << std::endl;

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
