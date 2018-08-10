#pragma once

#include <string>

namespace Coinbase
{
	struct StreamCallbacks
	{
		virtual void OnSnapshot(const std::string & product) = 0;
		// , const IterableResult<OrderBookItem> & bids, const IterableResult<OrderBookItem> & asks) = 0;
		//virtual void OnUpdate(const QString & product, const IterableResult<OrderBookChange> & changes) = 0;
		//virtual void OnHeartbeat(const QDateTime & serverTime) = 0;
		//virtual void OnTick(const Tick & tick) = 0;
		//virtual void OnStateChanged(ConnectedState state) = 0;

	protected:
		virtual ~StreamCallbacks() = default;
	};
}