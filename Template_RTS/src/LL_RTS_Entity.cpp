#include "LL_RTS_Entity.h"
#include "LL_RTS_GameMaster.h"

namespace LL::RTS {

	Entity::Entity(GameMaster* GM, Maths::Position position, EntityId id)
		: GM(GM), position(position), id(id) {
	}

	Entity::~Entity() {
		if (GM) GM->RemoveEntity(id);
	}

	void Entity::SetId(EntityId id)
	{
		this->id = id;
	}


}