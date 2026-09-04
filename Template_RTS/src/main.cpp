#include <iostream>
#include <string>

#include "LL_RTS_Player.h"


int main() {
    LL::RTS::Player player;
    auto bob = std::make_unique<LL::RTS::Character>();
    bob->SetName("Bob Dylan");
    auto lili = std::make_unique<LL::RTS::Character>();
    lili->SetName("Lili");
    auto charlie = std::make_unique<LL::RTS::Character>();
    charlie->SetName("Charlie Chaplin");

    // SelectCharacter replaces selection. Use AddToSelection to multi-select.
    player.SelectCharacter(bob.get());
    player.AddToSelection(charlie.get());

    // Keep target selection separate
    player.SelectTarget(lili.get());

    // Give a group move order to Bob and Charlie
    player.GiveOrder({
        LL::RTS::OrderType::Move,
        {20, 20},
        nullptr
        });

    // Lili moves independently
    lili->AddOrder({
        LL::RTS::OrderType::Move,
        {30, 30},
        nullptr
        });

    player.GiveOrder({
        LL::RTS::OrderType::Attack,
        {0,0},
        lili.get()
        });

    float gameSpeedTest = 0.5f; // How many seconds in one tick?

    for (int i = 0; i < 50; i++) {
        // Tick time
        lili->Update(gameSpeedTest);
        lili->LogPosition();

        bob->Update(gameSpeedTest);
        bob->LogPosition();

        charlie->Update(gameSpeedTest);
        charlie->LogPosition();
    }

    return 0;
}