#pragma once

#include <vector>
#include <memory>

#include "LL_RTS_Enumerators.h"
#include "LL_RTS_ResourcePool.h"

namespace LL::RTS {

    class GameMaster;
    struct Order;
    class Character;

    /** Represents a player: owns resources and controls a selection of characters. */
    class Player {
    public:
        Player();
        /** Convenience constructor used by main.cpp. */
        Player(GameMaster* GM);
        /** Constructor called by the GameMaster to initialize a player with its id. */
        Player(GameMaster* GM, uint32_t id);

        void Update();

        /** Selects a single Character, replacing the current selection. */
        void SelectCharacter(Character* inCharacter);

        /** Adds a Character to the current selection. */
        void AddToSelection(Character* inCharacter);
        /** Removes a Character from the current selection. */
        void DeselectCharacter(Character* inCharacter);
        /** Clears the current selection. */
        void ClearSelection();

        /** Sets the current attack target. */
        void SelectTarget(Character* inTarget);
        /** Clears the current attack target if it matches inTarget. */
        void DeselectTarget(Character* inTarget);

        /** Broadcasts an order to all selected characters. */
        void GiveOrder(Order order);

        /** Returns the selected character at the given index. */
        Character* GetSelectedCharacter(size_t index);
        /** Returns all currently selected characters. */
        const std::vector<Character*>& GetSelectedCharacters() const;

        /** Attempts to pay a unit's maintenance cost; returns false if the player can't afford it. */
        bool PayUnit(ResourcePool* resourceCost);

        /** Attempts to pay a building's maintenance cost; returns false if the player can't afford it. */
        bool PayBuilding(ResourcePool resourceCost);

        /**
         * Subtracts the given amount of a resource from the player's pool, e.g. for purchases or transfers.
         * @return False if the player doesn't have enough to pay; the pool is left unchanged in that case.
         */
        bool UseResource(Resource resource, uint32_t amount);

        /**
         * Adds the given amount of a resource to the player's pool.
         * @return The resulting total amount of that resource.
         */
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