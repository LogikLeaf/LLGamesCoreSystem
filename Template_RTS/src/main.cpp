#include <iostream>
#include <string>
#include <vector>
#include <span>

#include "LL_RTS_GameMaster.h"
#include "LL_RTS_Player.h"
#include "LL_RTS_Character.h"

int main() {
    LL::RTS::GameMaster GM;
    LL::RTS::Player player(&GM);

    // GameMaster owns every Character's lifetime via its EntitySlots;
    // these are non-owning handles, valid as long as GM is alive and the slot isn't reused
    LL::RTS::Character* bob = GM.SpawnCharacter("Bob Dylan");
    LL::RTS::Character* lili = GM.SpawnCharacter("Lili");
    LL::RTS::Character* charlie = GM.SpawnCharacter("Charlie Chaplin");
    LL::RTS::Character* nelson = GM.SpawnCharacter("Nelson Mandela");
    

    // Select Bob, Charlie and Nelson as a group (will receive formation offsets)
    player.SelectCharacter(bob);
    player.AddToSelection(charlie);
    player.AddToSelection(nelson);

    // Move group to a far destination so formation is visible
    player.GiveOrder({
        LL::RTS::OrderType::Move,
        {50.0f, 0.0f},
        nullptr
        });

    // Then attack Lili together
    player.GiveOrder({
        LL::RTS::OrderType::Attack,
        {0,0},
        lili
        });

    // Lili moves independently elsewhere
    lili->AddOrder({
        LL::RTS::OrderType::Move,
        {30.0f, 10.0f},
        nullptr
        });

    float gameSpeedTest = 1.0f; // seconds per tick

    // Run a few dozen ticks and log positions to watch formation.
    // GameMaster::Update drives every spawned Character, building a consistent
    // per-tick neighbor snapshot for local avoidance.
    for (int i = 0; i < 40; ++i) {
        std::cout << "---- tick " << i << " ----\n";

        bob->LogPosition();
        charlie->LogPosition();
        nelson->LogPosition();
        lili->LogPosition();

        GM.Update(gameSpeedTest);

    }

    Log("main completed, returning now.");
    std::cout << std::flush;
    return 0;
}