#pragma once
#include <vector>
#include <memory>

#include "LL_RTS_Structs.h"
#include "LL_RTS_EntitySlot.h"
#include "LL_RTS_Player.h"

namespace LL::RTS {

	//class Player;
	class Entity;
	//class EntitySlot;
	class Character;

	class GameMaster {
	public:
		// Starts the game and initializes everything
		void StartGame();

		// Add a player to this game
		void AddPlayer(Player player);

		// To update the state of an entity, but should be used to communicate with the server?
		// Need to think about it
		//void UpdateState()

		void RemoveEntity(EntityId entityId);

		// When a Player asks a specific Character to do something
		void AddOrder(Order order, std::vector<Character*> character);

		// A new slot is free, add it to the freeSlots list, so we know we can use it
		void AddAvailableSlot(uint32_t slotId);
		// A slot just got filled with an entity, remove it from the freeSlots list
		void RemoveAvailableSlot(uint32_t slotId);

		// Spawn Character and add it to a free slot
		void SpawnCharacter(Character character);

	private:
		std::vector<std::unique_ptr<EntitySlot>> entities;
		std::vector<uint32_t> freeSlots;

		std::vector<std::unique_ptr<Player>> players;


	};

}