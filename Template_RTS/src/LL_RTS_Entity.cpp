#include "LL_RTS_Entity.h"
#include "LL_RTS_GameMaster.h"

namespace LL::RTS {

	Entity::Entity(GameMaster* GM, Maths::Vector2D position, uint32_t id)
		: GM(GM), position(position), id(id) {}

	Entity::~Entity() {
		GM->RemoveEntity(this);
	}


}