#include "DaughterBoards.hpp"

#include <iostream>
#include <bitset> // to display values in binary, e.g. std::cout << std::bitset<8>(myNumber) << std::endl
#include <unistd.h> // for usleep

DaughterBoards::DaughterBoards() {
    init();
}

void DaughterBoards::test() {
    
    turnOn();
    sleep(2); // 2 seconds
    turnOff();
    sleep(2); // 2 seconds
    turnOn();
}

void DaughterBoards::init(){
    wiringPiSetupGpio();
    pinMode (DAUGHTER_BOARD1_GPIO_PIN, OUTPUT);
    pinMode (DAUGHTER_BOARD2_GPIO_PIN, OUTPUT);
    turnOn(); // on by default
};

void DaughterBoards::turnOn(){
    digitalWrite (DAUGHTER_BOARD1_GPIO_PIN, HIGH);
    digitalWrite (DAUGHTER_BOARD2_GPIO_PIN, HIGH);
	std::cout << "turn on daughter boards" << std::endl;
};

void DaughterBoards::turnOff(){
    digitalWrite (DAUGHTER_BOARD1_GPIO_PIN, LOW);
    digitalWrite (DAUGHTER_BOARD2_GPIO_PIN, LOW);
	std::cout << "turn off daughter boards" << std::endl;
};
