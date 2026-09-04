#include <iostream>
#include <string>

#include "LL_RTS_Player.h"



int main() {
    LL::RTS::Player player;
    auto bob = std::make_unique<LL::RTS::Character>();
    bob->SetName("Bob");
    auto lili = std::make_unique<LL::RTS::Character>();
    lili->SetName("Lili");

    player.SelectCharacter(bob.get());
    player.SelectTarget(lili.get());


    player.GiveOrder({
        LL::RTS::OrderType::Move,
        {10, 0},
        nullptr
        });

    player.GiveOrder({
        LL::RTS::OrderType::Move,
        {8, 12},
        nullptr
        });

    player.GiveOrder({
        LL::RTS::OrderType::Attack,
        {0,0},
        lili.get()
        });

    lili->AddOrder({
        LL::RTS::OrderType::Move,
        {30, 30},
        nullptr
        });

    float gameSpeedTest = 0.5f; // How many seconds in one tick?

    for (int i = 0; i < 50; i++) {
        // Tick time
        lili->Update(gameSpeedTest);
        lili->LogPosition();

        bob->Update(gameSpeedTest);
        bob->LogPosition();
    }


    return 0;
}




