#include <iostream>
#include <string>

#include "LL_RTS_GameMaster.h"
#include "LL_RTS_Player.h"
#include "LL_RTS_Character.h"

// Demo entry point: spawns a few characters and exercises orders,
// formation movement, and combat over a fixed number of ticks.
int main() {
    LL::RTS::GameMaster GM;
    LL::RTS::Player player(&GM);

    auto bob = std::make_unique<LL::RTS::Character>();
    bob->SetName("Bob Dylan");

    auto lili = std::make_unique<LL::RTS::Character>();
    lili->SetName("Lili");

    auto charlie = std::make_unique<LL::RTS::Character>();
    charlie->SetName("Charlie Chaplin");

    auto nelson = std::make_unique<LL::RTS::Character>();
    nelson->SetName("Nelson Mandela");

    // Select Bob and Charlie as a group (will receive formation offsets)
    player.SelectCharacter(bob.get());
    player.AddToSelection(charlie.get());
    player.AddToSelection(nelson.get());

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
        lili.get()
        });

    // Lili moves independently elsewhere
    player.SelectCharacter(lili.get());
    lili->AddOrder({
        LL::RTS::OrderType::Move,
        {30.0f, 10.0f},
        nullptr
        });

    float gameSpeedTest = 0.5f; // seconds per tick

    // Run a few dozen ticks and log positions to watch formation
    for (int i = 0; i < 40; ++i) {
        bob->Update(gameSpeedTest);
        bob->LogPosition();

        charlie->Update(gameSpeedTest);
        charlie->LogPosition();

        nelson->Update(gameSpeedTest);
        nelson->LogPosition();

        lili->Update(gameSpeedTest);
        lili->LogPosition();

        std::cout << "---- tick " << i << " ----\n";
    }

    return 0;
}