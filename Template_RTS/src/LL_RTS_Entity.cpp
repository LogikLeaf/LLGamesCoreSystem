#include "LL_RTS_Entity.h"
#include "LL_RTS_GameMaster.h"

namespace LL::RTS {

	Entity::Entity(GameMaster* GM, EntityId id)
		: GM(GM), id(id) {
	}

	Entity::~Entity() {
		if (GM) GM->RemoveEntity(id);
	}

	void Entity::SetId(EntityId id)
	{
		this->id = id;
	}


}