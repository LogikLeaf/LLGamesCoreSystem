#pragma once

/*
* Used by GameManager to store evey entity. It's a smart id manager
* that ensures the entities references to remain valid throughout the game.
*/

#include <memory>

#include "LL_RTS_Entity.h"



namespace LL::RTS {

	class GameMaster;

	class EntitySlot {
	public:
		// Constructor to get the mandatory GameMaster's reference
		EntitySlot(GameMaster* GM, uint32_t id);

		// Getters
		// Get the Entity reference
		Entity* GetEntity();
		// Get the EntityId
		EntityId GetId() const;


		// Returns true if an entity is occupying this slot
		bool IsOccupied();

		// Destroy the owned Entity, and make the slot available
		void Destroy();

		// Add a new Entity
		void Add(std::unique_ptr<Entity> entity);

	private:
		EntityId entityId;
		std::unique_ptr<Entity> entity;

		GameMaster* GM;
	};

}