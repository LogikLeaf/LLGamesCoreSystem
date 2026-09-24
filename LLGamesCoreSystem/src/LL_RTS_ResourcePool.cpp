#include "LL_RTS_ResourcePool.h"

#include <limits>


namespace LL::RTS {

	ResourcePool::ResourcePool() {
		for (size_t i = 0; i < static_cast<size_t>(Resource::Count); i++) {
			resourcePool[i] = 0;
		}
	}

	void ResourcePool::Adjust(Resource resource, int64_t amount) {
        size_t idx = static_cast<size_t>(resource);
        int64_t current = static_cast<int64_t>(resourcePool[idx]);
        int64_t result = current + amount;

        if (result <= 0) resourcePool[idx] = 0;
        else if (result > static_cast<int64_t>(std::numeric_limits<uint32_t>::max()))
            resourcePool[idx] = static_cast<uint32_t>(std::numeric_limits<uint32_t>::max());
        else
            resourcePool[idx] = static_cast<uint32_t>(result);
    }

    uint32_t ResourcePool::Get(Resource resource) {
        return resourcePool[static_cast<size_t>(resource)];
    }

}