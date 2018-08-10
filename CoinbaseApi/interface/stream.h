#pragma once

namespace Coinbase
{
	struct Stream
	{
		//        virtual void Subscribe(const Subscription & subscription) = 0;
		//        virtual void Unsubscribe(const Subscription & subscription) = 0;
		virtual ~Stream() = 0 {} // https://eli.thegreenplace.net/2010/11/13/pure-virtual-destructors-in-c
	};
}