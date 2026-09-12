#pragma once

#include "LL_RTS_Enumerators.h"

namespace LL::RTS {

	/** Holds a player's stock of each Resource type. */
	class ResourcePool {
	public:

		ResourcePool();

		/** Adds (or subtracts, if negative) an amount to the given resource. */
		void Adjust(Resource resource, int64_t amount);

		/** Returns the current amount of the given resource. */
		uint32_t Get(Resource resource);

	private:
		uint32_t resourcePool[static_cast<size_t>(Resource::Count)];
	};

}