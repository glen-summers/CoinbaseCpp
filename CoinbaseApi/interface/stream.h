#pragma once

namespace Coinbase
{
	struct Stream
	{
		virtual void Start() = 0;
		//        virtual void Subscribe(const Subscription & subscription) = 0;
		//        virtual void Unsubscribe(const Subscription & subscription) = 0;

		// not rerally needed as shared_ptr magic will call derived dtor
		virtual ~Stream() = default;
	};
}