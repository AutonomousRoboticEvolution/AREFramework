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

//    std::cout << "resetting\n" ;
    write8(COMMAND_DEVICE_RESET);
    write8(COMMAND_1_WIRE_RESET);
    waitFor1WireToFinish();

    setI2CReadPointer(POINTER_CODE_DEVICE_CONFIGURATION);
//    std::cout << "device config reads: " << std::bitset<8>(read8()) << std::endl;

    setI2CReadPointer(POINTER_CODE_STATUS );
//    std::cout << "status: " << std::bitset<8>(read8()) << std::endl;

//    std::cout << "setting config to: " << std::bitset<8>(DS2484_DEFAULT_CONFIGURATION_DATA) << std::endl;
    waitFor1WireToFinish();
    write8To(COMMAND_WRITE_DEVICE_CONFIGURATION , DS2484_DEFAULT_CONFIGURATION_DATA);

    setI2CReadPointer(POINTER_CODE_DEVICE_CONFIGURATION);
//    std::cout << "device config reads: " << std::bitset<8>(read8()) << std::endl;
//    std::cout << "END OF INIT" << std::endl<< std::endl;

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

void BatteryMonitor::setI2CReadPointer(int pointerCode){
    write8To( COMMAND_SET_READ_POINTER, pointerCode );
}

int BatteryMonitor::read1WireByte() {
    write8(COMMAND_1_WIRE_READ_BYTE);
    waitFor1WireToFinish();
    setI2CReadPointer( POINTER_CODE_READ_DATA );
    return read8();
}

void BatteryMonitor::waitFor1WireToFinish() {
    setI2CReadPointer(POINTER_CODE_STATUS);
    while(read8() & 0x01){}
}


float BatteryMonitor::measureTemperature() {

    reset1Wire();
    skipROM();
    write1WireByte({BATTERY_MONITOR_CONVERT_TEMPERATURE}); //convert T
    waitForDS2438ToStopBeingBusy();

    // note the waitForDS2438ToStopBeingBusy() will leave the last page retrieved as page0, so we don't need to update it again
    int16_t temperatureData = pageDataFromDS2438[2]<<8 | pageDataFromDS2438[1];
    bool signBit = temperatureData & 0x8000 >> 15; // zero if positive, one if negative (2's compliment)
    if (signBit)  {// value is negative (wow it's cold in here!) - do two's compliment by flipping bits and adding one
        temperatureData ^= 0xFFFF; // XOR with all ones to flip every bit
        temperatureData += 1 ; // add one
        return -1 * (float)temperatureData/256;
    } else{ // value is positive
        return (float) temperatureData/256;
    }
}

void BatteryMonitor::uploadNewConfig(uint8_t statusConfigByte){
    bool uploadSuccessful = false;
    int retries =0;
    while (!uploadSuccessful) {
        // upload the new status byte to the scratchpad
        reset1Wire();
        skipROM();
        write1WireByte(BATTERY_MONITOR_WRITE_SCRATCHPAD); //write scratchpad
        write1WireByte(0x00); //page 0
        write1WireByte(statusConfigByte);
        reset1Wire();

        // check the scratchpad value is correct
        updateReadPageDataFromScratchpad(0x00); // read page0 which contains the status/config byte
        if (pageDataFromDS2438[0] == statusConfigByte){ // 0th byte is the status/config byte
            // the upload happened correctly
            uploadSuccessful=true;
        }else{
            // this is bad
            retries++;
            if (retries>BATTERY_MONITOR_UPLOAD_MAX_RETRIES){
                // this is very bad
                throw std::runtime_error("Uploading new config to the battery monitor has failed. Check for problems with the 1Wire bus, or bugs in this code!");
            }
        }
    }
    // upload successful to scratchpad, so now need to copy from scratchpad into the actual memory
    reset1Wire();
    skipROM();
    write1WireByte(BATTERY_MONITOR_COPY_SCRATCHPAD_TO_MEMORY); //copy scratchpad to memory
    write1WireByte(0x00); //select page 0
    write1WireByte(statusConfigByte); // issue bytes in order, starting with 0th, which is the only one we need to send
    reset1Wire();
}

uint16_t BatteryMonitor::measure5VBusVoltage() {
    //check if the battery monitor is currently in bus voltage mode
    updateReadPageData(0x00); // read page0 which contains the status/config byte
    uint8_t statusConfigByte = pageDataFromDS2438[0]; // the 0th byte is the status/config byte

    if ((statusConfigByte & BATTERY_MONITOR_BITMASK_CONFIG_VOLTAGE_AD) != 0){ // if not already in bus voltage mode, we need to change it
        // modify status byte to include battery voltage mode = 0 (for 5V bus measurement)
        statusConfigByte = statusConfigByte & (BATTERY_MONITOR_BITMASK_CONFIG_VOLTAGE_AD^0xF); // keep all bit the same, except the VOLTAGE_AD which becomes 0
        uploadNewConfig(statusConfigByte);
    }
    // now in bus voltage measurement mode :)

    // do the voltage measurement
    reset1Wire();
    skipROM();
    write1WireByte({BATTERY_MONITOR_CONVERT_VOLTAGE}); //convert V
    waitForDS2438ToStopBeingBusy();

    // note the waitForDS2438ToStopBeingBusy() will leave the last page retrieved as page0, so we don't need to update it again
    int16_t voltageData = pageDataFromDS2438[4]<<8 | pageDataFromDS2438[3];
    return voltageData;
}

uint16_t BatteryMonitor::measureBatteryVoltage() {
    //check if the battery monitor is currently in battery mode
    updateReadPageData(0x00); // read page0 which contains the status/config byte
    uint8_t statusConfigByte = pageDataFromDS2438[0]; // the 0th byte is the status/config byte

    if ((statusConfigByte & BATTERY_MONITOR_BITMASK_CONFIG_VOLTAGE_AD) == 0){ // if not already in battery mode, we need to change it
        // modify status byte to include battery voltage mode = 1
        statusConfigByte = statusConfigByte | BATTERY_MONITOR_BITMASK_CONFIG_VOLTAGE_AD; // keep all bit the same, except the VOLTAGE_AD which becomes 1
        uploadNewConfig(statusConfigByte);
    }
    // now in battery measurement mode :)

    // do the voltage measurement
    reset1Wire();
    skipROM();
    write1WireByte({BATTERY_MONITOR_CONVERT_VOLTAGE}); //convert V
    waitForDS2438ToStopBeingBusy();

    // note the waitForDS2438ToStopBeingBusy() will leave the last page retrieved as page0, so we don't need to update it again
    int16_t voltageData = pageDataFromDS2438[4]<<8 | pageDataFromDS2438[3];
    return voltageData;
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
        bool temperatureBusyFlag = (statusByte & BATTERY_MONITOR_BITMASK_CONFIG_TEMPERATURE_BUSY) >> 4;
        bool ADConverterBusyFlag = (statusByte & BATTERY_MONITOR_BITMASK_CONFIG_AD_BUSY) >> 6;
        if (!temperatureBusyFlag and !ADConverterBusyFlag){waiting=false;}
    }
}

void BatteryMonitor::updateReadPageDataFromScratchpad(int pageNumber){
    int CRCcheck = 1; // this will become zero once the CRC test is passed, so initialse to something non-zero
    int CRCRetriesCounter = 0;
    while (CRCcheck != 0 ) {
        // command the reading of the scratchpad
        reset1Wire();
        skipROM();
        write1WireByte(BATTERY_MONITOR_READ_SCRATCHPAD); // Issue Read SP 00h command
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

void BatteryMonitor::updateReadPageData(int pageNumber){

    // first, tell DS2438 to copy data from memory onto "scratchpad"
    reset1Wire();
    skipROM();
    write1WireByte(BATTERY_MONITOR_COPY_MEMORY_TO_SCRATCHPAD); //recall memory
    write1WireByte(pageNumber); // page number to access

    updateReadPageDataFromScratchpad(pageNumber);

}

#define TOPBIT   (0x01 << 8) // bitmask for extracting the leftmost bit
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
