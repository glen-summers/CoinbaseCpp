// Tests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#define BOOST_TEST_MODULE test module name
#include <boost/test/included/unit_test.hpp>

#include "..\CoinbaseApi\Heartbeat.h"

BOOST_AUTO_TEST_CASE(ParseHeartbeatJson)
{
	std::string data(R"({
    "type":"heartbeat",
    "last_trade_id":14275328,
    "product_id":"BTC-EUR",
    "sequence":3637742500,
    "time":"2018-05-03T19:29:24.335000Z"
})");

	rapidjson::Document doc;
	doc.Parse(data.c_str());
	auto hb = Heartbeat::Parse(doc);

	BOOST_TEST(hb.Id() == 14275328);
	BOOST_TEST(hb.Seq() == 3637742500);
}
