#pragma once

/*
* An Entity represents any object spawned in the world.
*/

#include <cstdint>
#include "LL_Maths.h"
#include "LL_RTS_Structs.h"


namespace LL::RTS {

	class GameMaster;

	class Entity {
	public:
		// An entity has necesserly a 2D world location
		Entity(GameMaster* GM, Maths::Vector2D position, EntityId id);
		~Entity();

		void SetId(EntityId id);

	protected:
		Maths::Vector2D position = { 0,0 };
		GameMaster* GM = nullptr;
		EntityId id = { 0,0 };
	};
}