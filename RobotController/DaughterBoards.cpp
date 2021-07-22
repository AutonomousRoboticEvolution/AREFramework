#include "DaughterBoards.hpp"

#include <iostream>
#include <bitset> // to display values in binary, e.g. std::cout << std::bitset<8>(myNumber) << std::endl
#include <unistd.h> // for usleep

DaughterBoards::DaughterBoards() {
    init();
}

void DaughterBoards::test() {
    
    turnOn();
    sleep(2);
    turnOn(LEFT);
    sleep(2);
    turnOn(RIGHT);
    sleep(2);
    turnOn(NONE);
    sleep(2);
    turnOn();
}

void DaughterBoards::init(){
    wiringPiSetupGpio();
    pinMode (DAUGHTER_BOARD1_GPIO_PIN, OUTPUT);
    pinMode (DAUGHTER_BOARD2_GPIO_PIN, OUTPUT);
    turnOn(); // on by default
};

void DaughterBoards::turnOn(boardSelection boardToEnable){
    bool debugPrinting=true;
    if (boardToEnable==BOTH){
        digitalWrite (DAUGHTER_BOARD1_GPIO_PIN, HIGH);
        digitalWrite (DAUGHTER_BOARD2_GPIO_PIN, HIGH);
	    if (debugPrinting) std::cout << "turn on both daughter boards" << std::endl;
    }
    else if (boardToEnable==LEFT){
        digitalWrite (DAUGHTER_BOARD1_GPIO_PIN, HIGH);
        digitalWrite (DAUGHTER_BOARD2_GPIO_PIN, LOW);
	    if (debugPrinting) std::cout << "turn on left daughter board" << std::endl;
    }
    else if (boardToEnable==RIGHT){
        digitalWrite (DAUGHTER_BOARD1_GPIO_PIN, LOW);
        digitalWrite (DAUGHTER_BOARD2_GPIO_PIN, HIGH);
	    if (debugPrinting) std::cout << "turn on right daughter board" << std::endl;
    }
    else {
        this->turnOff();
    }
};

void DaughterBoards::turnOff(){
    digitalWrite (DAUGHTER_BOARD1_GPIO_PIN, LOW);
    digitalWrite (DAUGHTER_BOARD2_GPIO_PIN, LOW);
	std::cout << "turn off daughter boards" << std::endl;
};
