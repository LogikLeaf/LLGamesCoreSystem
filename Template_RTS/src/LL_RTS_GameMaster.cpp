#include "LL_RTS_GameMaster.h"
#include "LL_RTS_Entity.h"
#include "LL_RTS_EntitySlot.h"
#include "LL_RTS_Character.h"

namespace LL::RTS {

	void GameMaster::StartGame() {
		// TODO: complete this
	}

	void GameMaster::SpawnCharacter(Character character) {
			// Create the character instance.
			auto characterEntity = std::make_unique<Character>(this);

			// Optionally copy some basic data from the passed-in 'character' value
			// TODO: to improve later
			characterEntity->SetName(character.GetName());
		
		if (freeSlots.empty()) { // No available slot
			// New slot id
			uint32_t newId = entities.size();
			// Create a new slot and add a Character entity to it
			auto slot = std::make_unique<EntitySlot>(this, newId);



			slot->Add(std::move(characterEntity));
			entities.push_back(std::move(slot));
		}
		else { // A slot is available
			// Add the new character to the first available slot
			entities.at(freeSlots.at(0))->Add(std::move(characterEntity));
		}
	}

	void GameMaster::RemoveEntity(EntityId entityId) {
		if (entities.at(entityId.id)->GetId().generation 
			== entityId.generation) entities.at(entityId.id)->Destroy();
	}

	
	void GameMaster::AddAvailableSlot(uint32_t slotId) {
		freeSlots.push_back(slotId);
	}

	void GameMaster::RemoveAvailableSlot(uint32_t slotId) {
		auto it = std::find(freeSlots.begin(), freeSlots.end(), slotId);
		if (it != freeSlots.end()) {
			freeSlots.erase(it);
		}
	}
}


// TODO:
// - Add order (call from Player)
// - Create Players with an id for each
//