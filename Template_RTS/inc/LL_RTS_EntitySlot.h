#pragma once

/*
* Used by GameManager to store evey entity. It's a smart id manager
* that ensures the entities references to remain valid throughout the game.
*/

#include <memory>

#include "LL_RTS_Entity.h"



namespace LL::RTS {

	class GameMaster;

	/** Owns a single Entity slot managed by the GameMaster, keeping its id stable while occupied. */
	class EntitySlot {
	public:
		EntitySlot();
		/** Creates a slot bound to a GameMaster and a given id. */
		EntitySlot(GameMaster* GM, uint32_t id);

		/** Returns the owned Entity, or nullptr if the slot is empty. */
		Entity* GetEntity();
		/** Returns this slot's EntityId. */
		EntityId GetId() const;

		/** Returns true if an entity currently occupies this slot. */
		bool IsOccupied();

		/** Destroys the owned Entity and frees the slot for reuse. */
		void Destroy();

		/** Assigns a new Entity to this slot. */
		void Add(std::unique_ptr<Entity> entity);

	private:
		EntityId entityId;
		std::unique_ptr<Entity> entity;

		GameMaster* GM;
	};

}