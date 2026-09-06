#pragma once

#include "LL_RTS_Enumerators.h"

namespace LL::RTS {

	class ResourcePool {
	public:
		
		ResourcePool();

		// Add or substract a chosen resource
		void Adjust(Resource resource, int64_t amount);
		// How much do we have of the specified resource
		uint32_t Get(Resource resource);

	private:
		uint32_t resourcePool[static_cast<size_t>(Resource::Count)];
	};

}