#pragma once
#include <vector>
#include <memory>

#include "LL_RTS_Structs.h"
#include "LL_RTS_EntitySlot.h"
#include "LL_RTS_Player.h"

namespace LL::RTS {

	class Entity;
	class Character;

	/** Owns all entities and players, and mediates orders between them. */
	class GameMaster {
	public:
		/** Starts the game and initializes everything. */
		void StartGame();

		/** Adds a player to this game. */
		void AddPlayer(Player player);

		// To update the state of an entity, but should be used to communicate with the server?
		// Need to think about it
		//void UpdateState()

		/** Frees the entity's slot, making its id available for reuse. */
		void RemoveEntity(EntityId entityId);

		/** Gives an order to the specified characters. */
		void AddOrder(Order order, std::vector<Character*> character);

		/** Marks a slot as free so it can be reused. */
		void AddAvailableSlot(uint32_t slotId);
		/** Marks a slot as occupied, removing it from the free list. */
		void RemoveAvailableSlot(uint32_t slotId);

		/** Spawns a Character into a free slot. */
		void SpawnCharacter(Character character);

	private:
		std::vector<std::unique_ptr<EntitySlot>> entities;
		std::vector<uint32_t> freeSlots;

		std::vector<std::unique_ptr<Player>> players;


	};

}