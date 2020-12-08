#include "DaughterBoards.hpp"

#include <iostream>
#include <bitset> // to display values in binary, e.g. std::cout << std::bitset<8>(myNumber) << std::endl
#include <unistd.h> // for usleep

DaughterBoards::DaughterBoards() {
    init();
}

void DaughterBoards::test() {
    std::cout << "turn on daughter boards" << std::endl;
    turnOn();
    usleep(200000); // 2 seconds
    std::cout << "turn off daughter boards" << std::endl;
    turnOff();
}

void DaughterBoards::init(){
    wiringPiSetupGpio();
    pinMode (DAUGHTER_BOARD1_GPIO_PIN, OUTPUT);
    pinMode (DAUGHTER_BOARD2_GPIO_PIN, OUTPUT);
    turnOff();
};

void DaughterBoards::turnOn(){
    digitalWrite (DAUGHTER_BOARD1_GPIO_PIN, HIGH);
    digitalWrite (DAUGHTER_BOARD2_GPIO_PIN, HIGH);
};

void DaughterBoards::turnOff(){
    digitalWrite (DAUGHTER_BOARD1_GPIO_PIN, LOW);
    digitalWrite (DAUGHTER_BOARD2_GPIO_PIN, LOW);
};
