#include "LL_RTS_GameMaster.h"
#include "LL_RTS_Entity.h"
#include "LL_RTS_Character.h"
#include "LL_RTS_EntitySlot.h"

#include <span>
#include <algorithm>

namespace LL::RTS {

	GameMaster::GameMaster() : grid(Maths::Position{ 0.0f, 0.0f,}, 100u, 100u) {
		grid.SpawnTiles();
	}

	GameMaster::~GameMaster() {
		Log("GameMaster destructor running");
	}

	Grid& GameMaster::GetGrid() {
		return grid;
	}

	

	void GameMaster::Update(float deltaTime) {
		// Snapshot every live character's movement agent so all Update() calls
		// this tick see consistent neighbor positions (no order-dependent bias).
		std::vector<Movement::MovingAgent*> agents;
		agents.reserve(entities.size());

		for (auto& slot : entities) {
			if (Character* character = slot ? dynamic_cast<Character*>(slot->GetEntity()) : nullptr) {
				agents.push_back(&character->GetMovingAgent());
			}
		}

		std::span<Movement::MovingAgent* const> neighborView(agents);

		for (auto& slot : entities) {
			if (Character* character = slot ? dynamic_cast<Character*>(slot->GetEntity()) : nullptr)
				character->Update(deltaTime, grid, neighborView);
			
		}
	}

	void GameMaster::StartGame() {
		// TODO: complete this
		// - Main menu
		// - Create Game with all the players
		// - Consider an AI "neutral" player
		// ...
	}

	void GameMaster::AddPlayer(Player player) {
		uint32_t newId = static_cast<uint32_t>(players.size());
		players.push_back(std::make_unique<Player>(this, newId));
		/* TODO: Here, set the player's variables */

	}

	void GameMaster::AddOrder(Order order, std::vector<Character*> characters) {
		for (Character* character : characters)
			if (character) character->AddOrder(order);
	}

	Character* GameMaster::SpawnCharacter(std::string name) {
		// Create the character instance.
		auto characterEntity = std::make_unique<Character>(this, name);

		Character* handle = nullptr;


		if (freeSlots.empty()) { // No available slot
			// New slot id
			uint32_t newId = static_cast<uint32_t>(entities.size());

			// Create a new slot and add a Character entity to it
			auto slot = std::make_unique<EntitySlot>(this, newId);

			// Update the Character's id
			characterEntity->SetId(slot->GetId());

			slot->Add(std::move(characterEntity));
			entities.push_back(std::move(slot));

			handle = static_cast<Character*>(entities[newId]->GetEntity());
		}

		else { // A slot is available
			uint32_t slotIndex = freeSlots.at(0);

			// Update the Character's id
			characterEntity->SetId(entities.at(slotIndex)->GetId());

			// Add the new character to the first available slot
			entities.at(slotIndex)->Add(std::move(characterEntity));

			handle = static_cast<Character*>(entities[slotIndex]->GetEntity());
		}

		return handle;
	}

	void GameMaster::RemoveEntity(EntityId entityId) {
		if (entityId.id >= entities.size()) return;

		auto& slot = entities[entityId.id];
		if (slot && slot->GetId().generation == entityId.generation) slot->Destroy();
	}

	
	void GameMaster::AddAvailableSlot(uint32_t slotId) {
		// Avoid duplicates
		if (std::find(freeSlots.begin(), freeSlots.end(), slotId) == freeSlots.end())
		freeSlots.push_back(slotId);
	}
	/*
	void GameMaster::RemoveAvailableSlot(uint32_t slotId) {
		auto it = std::find(freeSlots.begin(), freeSlots.end(), slotId);
		if (it != freeSlots.end()) {
			freeSlots.erase(it);
		}
	}

	void GameMaster::RemoveAvailableSlot(uint32_t slotId) {
		// Remove all occurrences of slotId in a single safe operation.
		auto newEnd = std::remove(freeSlots.begin(), freeSlots.end(), slotId);
		if (newEnd != freeSlots.end())
			freeSlots.erase(newEnd, freeSlots.end());
	}*/
	void GameMaster::RemoveAvailableSlot(uint32_t slotId) {
		auto newEnd = std::remove(freeSlots.begin(), freeSlots.end(), slotId);
		freeSlots.erase(newEnd, freeSlots.end());
	}
}


// TODO:
// - Add order (call from Player)
// - Create Players with an id for each
//