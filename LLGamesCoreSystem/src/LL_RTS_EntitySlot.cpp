#include "LL_RTS_EntitySlot.h"
#include "LL_RTS_GameMaster.h"

namespace LL::RTS {

	EntitySlot::EntitySlot(GameMaster* GM, uint32_t id) : GM(GM) {
		entityId = { id, 0 };
	}

	Entity* EntitySlot::GetEntity() {
		return entity.get();
	}
	EntityId EntitySlot::GetId() const {
		return entityId;
	}

	bool EntitySlot::IsOccupied() {
		return entity != nullptr;
	}

	void EntitySlot::Destroy() {
		entityId.generation++;
		if (GM) GM->AddAvailableSlot(entityId.id);
		entity.reset();
	}


	void EntitySlot::Add(std::unique_ptr<Entity> entity) {
		this->entity = std::move(entity);
		if (GM) GM->RemoveAvailableSlot(entityId.id);
	}

}