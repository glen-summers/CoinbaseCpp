#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // std::iterator used as base
#endif
#include <rapidjson/document.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <string>

class Heartbeat
{
	uint64_t const id;
	uint64_t const seq;

public:
	Heartbeat(uint64_t id, uint64_t seq)
		: id(id)
		, seq(seq)
	{}

	uint64_t Id() const { return id; }
	uint64_t Seq() const {return seq; }

	static Heartbeat Parse(const rapidjson::Document & d) // or just string? hide impl?
	{
		const auto & object = d.GetObject();
		auto id = object["last_trade_id"].GetUint64();
		auto seq = object["sequence"].GetUint64();

		return Heartbeat { id, seq };
	}
};