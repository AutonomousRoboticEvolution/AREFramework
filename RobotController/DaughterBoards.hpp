/**
	@file DaughterBoards.hpp
	@brief Header and documentation for DaughterBoards.cpp, for enable/disable of daughter boards
	@author Matt Hale
*/

#ifndef DAUGHTER_BOARDS_HPP
#define DAUGHTER_BOARDS_HPP

#define DAUGHTER_BOARD1_GPIO_PIN 17
#define DAUGHTER_BOARD2_GPIO_PIN 27

#include <wiringPi.h>

/**
	used to select between the two daghter boards.
    "left" and "right" are defined as the camera on the Head looking forward.
*/
enum boardSelection {LEFT, RIGHT, BOTH, NONE};

class DaughterBoards{
    public:
        DaughterBoards();

        void init();

        /**
         * quick demo, turn both boards on for a few seconds and then off
         */
        void test();

        /**
         * Enable the daughter boards
         */
        void turnOn(boardSelection boardToEnable=BOTH);

        /**
         * Disable the daughter boards
         */
        void turnOff();

};

#endif
