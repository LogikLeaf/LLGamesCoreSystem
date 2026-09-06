#pragma once

#include <vector>
#include <memory>

#include "LL_RTS_Enumerators.h"
#include "LL_RTS_ResourcePool.h"

namespace LL::RTS {

    class GameMaster;
    struct Order;
    class Character;
    //class ResourcePool;

    class Player {
    public:
        // Default constructor
        Player();
        // Convenience constructor used by main.cpp
        Player(GameMaster* GM);
        // This constructor is called by GM to initialize the player
        Player(GameMaster* GM, uint32_t id);

        // Tick Update
        void Update();

        // Select a single Character
        void SelectCharacter(Character* inCharacter);

        // Add/Remove Characters to selection
        void AddToSelection(Character* inCharacter);
        void DeselectCharacter(Character* inCharacter);
        void ClearSelection();

        // Targeting
        void SelectTarget(Character* inTarget);
        void DeselectTarget(Character* inTarget);

        // Broadcast an order to all selected Characters
        void GiveOrder(Order order);

        // Gettters
        Character* GetSelectedCharacter(size_t index);
        const std::vector<Character*>& GetSelectedCharacters() const;

        // Units maintenance cost - called by each owned unit
        bool PayUnit(ResourcePool* resourceCost);

        // Buildings maintenance cost - called by each owned building
        bool PayBuilding(ResourcePool resourceCost);

        // Subtract the specified resource from our pool. Called for one time paiements 
        // (e.g. When buying a new unit, or when giving money to another player)
        // Returns false and doesn't substract if player doesn't have enough to pay
        bool UseResource(Resource resource, uint32_t amount);

        // Give the specified resource to the player. Returns the final amount the player has.
        // Called by anything that produces any resource.
        uint32_t AddResource(Resource resource, uint32_t amount);

    private:
        std::vector<Character*> selectedCharacters;
        Character* target = nullptr;

        // The space between two units when moving in formation
        float formationSpacing = 3.0f;

        GameMaster* GM = nullptr;

        // Each player has an id so it can be referenced by the multiplayer system later
        uint32_t id = 0;

        // Player-owned resource pool. Reduced by PayUnit() and other purchases/transfers.
        std::unique_ptr<ResourcePool> resources;


    };



}