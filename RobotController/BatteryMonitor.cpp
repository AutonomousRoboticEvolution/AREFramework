/**
	@file BatteryMonitor.cpp
	@brief Implementation of BatteryMonitor methods
	@author Mike Angus
*/
#include "BatteryMonitor.hpp"

#include <iostream>
#include <bitset>
#include <unistd.h> // for usleep



BatteryMonitor::BatteryMonitor() : I2CDevice(BATTERY_MONITOR_I2C_ADDRESS) {
    //Calls the I2CDevice constructor
}


void BatteryMonitor::init() {

//    crcInit();

    std::cout << "resetting\n" ;
    write8(COMMAND_DEVICE_RESET);
    write8(COMMAND_1_WIRE_RESET);
    waitFor1WireToFinish();

    setReadPointer(POINTER_CODE_DEVICE_CONFIGURATION);
    std::cout << "device config read: " << std::bitset<8>(read8()) << std::endl;

    setReadPointer(POINTER_CODE_STATUS );
    std::cout << "status: " << std::bitset<8>(read8()) << std::endl;

    std::cout << "setting config to: " << std::bitset<8>(DS2484_DEFAULT_CONFIGURATION_DATA) << std::endl;
    waitFor1WireToFinish();
    write8To(COMMAND_WRITE_DEVICE_CONFIGURATION , DS2484_DEFAULT_CONFIGURATION_DATA);

    setReadPointer(POINTER_CODE_DEVICE_CONFIGURATION);
    std::cout << "device config read: " << std::bitset<8>(read8()) << std::endl;

    std::cout << "END OF INIT" << std::endl<< std::endl;

}

void BatteryMonitor::reset1Wire(){
//    std::cout << "1Wire reset/presence" << std::endl;
    waitFor1WireToFinish();
    write8(COMMAND_1_WIRE_RESET); // do the 1Wire reset-presence sequence
}
void BatteryMonitor::skipROM(){
//    std::cout << "Skip ROM" << std::endl;
    write1WireByte(0xCC); //skipROM
}

void BatteryMonitor::write1WireByte(int data) {
//    std::cout << "1Wire write: "<< std::hex << data << std::endl;
    waitFor1WireToFinish();
    write8To(COMMAND_1_WIRE_WRITE_BYTE,data);
}

void BatteryMonitor::setReadPointer(int pointerCode){
    write8To( COMMAND_SET_READ_POINTER, pointerCode );
}

int BatteryMonitor::read1WireByte() {
    write8(COMMAND_1_WIRE_READ_BYTE);
    waitFor1WireToFinish();
    setReadPointer( POINTER_CODE_READ_DATA );
    return read8();
}

void BatteryMonitor::waitFor1WireToFinish() {
    setReadPointer(POINTER_CODE_STATUS);
    while(read8() & 0x01){}
}


int BatteryMonitor::measureTemperature() {
    std::cout << "Measure temperature" << std::endl;
    reset1Wire();
    skipROM();
    write1WireByte({0x44}); //convert T
    waitForDS2438ToStopBeingBusy();
    return 0;
}

int BatteryMonitor::measureVoltage() {
    std::cout << "Measuring battery voltage" << std::endl;
    reset1Wire();
    skipROM();
    write1WireByte({0xB4}); //convert V
    waitForDS2438ToStopBeingBusy();

    // pageDataFromDS2438 will already have been updated in waitForDS2438ToStopBeingBusy(), so it already has page0 stored:
    std::cout << "--- page 0"<<" ---"<<std::endl;
    for(int i=0;i<8;i++){
        std::cout << i<<" ";
        std::cout << std::bitset<8>( pageDataFromDS2438[i] ) << std::endl;
    }

    return 0;
}

void BatteryMonitor::printAllPages() {

    for (int i_page=0; i_page<3; i_page++){
        std::cout << "--- page "<<i_page<<" ---"<<std::endl;
        updateReadPageData(i_page);
        for(int i=0;i<8;i++){
            std::cout << i<<" ";
            std::cout << std::bitset<8>( pageDataFromDS2438[i] ) << std::endl;
        }
    }
}

void BatteryMonitor::waitForDS2438ToStopBeingBusy() {
    bool waiting = true;
    while(waiting) {
        updateReadPageData(0x00); // read page0, which contains status flags
        int statusByte = pageDataFromDS2438[0]; // extract the status byte (byte 0)
        bool temperatureBusyFlag = (statusByte & 0b00010000) >> 4;
        bool ADConverterBusyFlag = (statusByte & 0b01000000) >> 6;
        if (!temperatureBusyFlag and !ADConverterBusyFlag){waiting=false;}
    }
}

void BatteryMonitor::updateReadPageData(int pageNumber){

    int CRCcheck = 1; // this will become zero once the CRC test is passed, so initialse to something non-zero
    int CRCRetriesCounter = 0;
    while (CRCcheck != 0 ) {

        // first, tell DS2438 to copy data from memory onto "scratchpad"
        reset1Wire();
        skipROM();
        write1WireByte(0xB8); //recall memory
        write1WireByte(pageNumber); // page number to access

        // then command the reading of the scratchpad
        reset1Wire();
        skipROM();
        write1WireByte(0xBE); // Issue Read SP 00h command
        write1WireByte(pageNumber); // page number to access

        // read expected 8 byte return:
        waitFor1WireToFinish();
        for (int i = 0; i < 8; i++) { // loop through each byte in the page
            pageDataFromDS2438[i] = read1WireByte();
        }
        // read the "extra" 9th byte, which is the CRC value:
        int receivedCRC = read1WireByte();

        // finish the conversation:
        reset1Wire();

        // check the CRC (the result should be zero):
        CRCcheck = doCRCCheck(pageDataFromDS2438, 8, receivedCRC);
        if (CRCcheck != 0) {
            CRCRetriesCounter++;
            if (CRCRetriesCounter>CRC_CHECK_MAX_RETRIES) { // time to give up
                throw std::runtime_error("CRC check on reading data from the battery monitor has failed. Increase CRC_CHECK_MAX_RETRIES or check for problems with the 1Wire bus.");
            }
        }
    } // CRC check now passed
}

//#define WIDTH    9
#define TOPBIT   (0x01 << 8)
int BatteryMonitor::doCRCCheck(int message[], int nBytes, int receivedCRC)
{
    int remainder = 0x00;

    for (int iByte = 0; iByte <= nBytes; ++iByte){
        int thisByte;
        if (iByte == nBytes) { thisByte =receivedCRC; } // effectively appending the receivedCRC to the message; to compute a new CRC, set this to be zero
        else{ thisByte = message[iByte]; }
//        else{ thisByte = message[nBytes-iByte-1]; } // last byte considered first


        for (int iBit = 0; iBit < 8; ++iBit) {
//            int thisBit = (thisByte >> (7-iBit)) & 1;
            int thisBit = (thisByte >> iBit) & 1; // LSB first
            remainder = ((remainder << 1) | thisBit);
            if (remainder & TOPBIT) {
                remainder ^= CRC_POLYNOMIAL;
            }
        }
    }
    return remainder;

}