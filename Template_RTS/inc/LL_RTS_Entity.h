#pragma once

#include <cstdint>
#include "LL_Maths.h"
#include "LL_RTS_Structs.h"


namespace LL::RTS {

	class GameMaster;

	/** Base class for any object spawned in the world; owns a position and a unique id. */
	class Entity {
	public:
		Entity(GameMaster* GM, Maths::Position position, EntityId id);
		/** Notifies the GameMaster that this entity's id is now free. */
		~Entity();

		/** Sets this entity's unique id. */
		void SetId(EntityId id);

	protected:
		Maths::Position position = { 0,0 };
		GameMaster* GM = nullptr;
		EntityId id = { 0,0 };
	};
}