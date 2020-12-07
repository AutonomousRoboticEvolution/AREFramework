/**
	@file BatteryMonitor.cpp
	@brief handles the DS2438Z+ battery monitor chip, via I2C converter
	@author Matt Hale
*/
#include "BatteryMonitor.hpp"

#include <iostream>
#include <bitset> // to display values in binary, e.g. std::cout << std::bitset<8>(myNumber) << std::endl
#include <unistd.h> // for usleep


BatteryMonitor::BatteryMonitor() : I2CDevice(BATTERY_MONITOR_I2C_ADDRESS) {
    //Calls the I2CDevice constructor
    init();
}

void BatteryMonitor::testingDisplayBatteryLevels(){


    uint16_t batteryVoltage = measureBatteryVoltage();
    uint16_t busVoltage = measure5VBusVoltage();
    int currentMilliAmps = measureCurrent();
    int batteryChargeRemaining = measureBatteryChargeRemaining();
    float temperature = measureTemperature();

    std::cout << "temperature: "<< temperature <<" degreesC"<<std::endl;
    std::cout << "batteryVoltage:         " << std::bitset<8>(batteryVoltage) << " = "<<batteryVoltage/100.0<<"V"<< std::endl;
    std::cout << "busVoltage:             " << std::bitset<8>(busVoltage) << " = "<<busVoltage/100.0<<"V"<< std::endl;
    std::cout << "current:                " << std::bitset<8>(currentMilliAmps) << " = " << currentMilliAmps <<"mA"<< std::endl;
    std::cout << "batteryChargeRemaining: " << std::bitset<8>(batteryChargeRemaining) << " = " << batteryChargeRemaining <<"mAh" <<std::endl;

    std::cout << std::endl<< "====== starting  ======" << std::endl<< std::endl;

    std::cout <<"Voltage\t|\tCurrent\t|\tICA"<<std::endl;
    int i=0;
    while (i<99){
        usleep(1000000);
        i++;
        if (i%10==0){std::cout <<"Voltage\t|\tCurrent\t|\tICA"<<std::endl;}
        batteryVoltage = measureBatteryVoltage();
        currentMilliAmps = measureCurrent();
        batteryChargeRemaining = measureBatteryChargeRemaining();
        std::cout << (float)batteryVoltage/100 << "V\t|\t" << currentMilliAmps << "mA\t|\t" << batteryChargeRemaining << "mAh" << std::endl;
    }
}

void BatteryMonitor::init() {

//    std::cout << "resetting i2c to 1Wire converter\n" ;
    write8(COMMAND_DEVICE_RESET);
    write8(COMMAND_1_WIRE_RESET);
    waitFor1WireToFinish();

    setI2CReadPointer(POINTER_CODE_DEVICE_CONFIGURATION);
//    std::cout << "old config reads: " << std::bitset<8>(read8()) << std::endl;

    setI2CReadPointer(POINTER_CODE_STATUS );
//    std::cout << "status: " << std::bitset<8>(read8()) << std::endl;

//    std::cout << "setting config to: " << std::bitset<8>(DS2484_DEFAULT_CONFIGURATION_DATA) << std::endl;
    waitFor1WireToFinish();
    write8To(COMMAND_WRITE_DEVICE_CONFIGURATION , DS2484_DEFAULT_CONFIGURATION_DATA);

}

void BatteryMonitor::reset1Wire(){
//    std::cout << "1Wire reset/presence" << std::endl;
    waitFor1WireToFinish();
    write8(COMMAND_1_WIRE_RESET); // do the 1Wire reset-presence sequence
}
void BatteryMonitor::skipROM(){
//    std::cout << "Skip ROM" << std::endl;
    write1WireByte(BATTERY_MONITOR_SkIP_ROM); //skipROM
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
    while(read8() & 0x01){} // LSB is the 1Wire status bit, zero once 1Wire conversation is finished
}

int BatteryMonitor::measureCurrent() {
    //get raw current data from bytes 5 and 6 of page0 (see datasheet page 16):
    updateReadPageData(0x00);
    int16_t currentDataRaw = pageDataFromDS2438[6]<<8 | pageDataFromDS2438[5];
    //std::cout <<"raw current data:  "<< std::bitset<16>( currentDataRaw ) << std::endl;

    // return converted to milliAmps for easier to use:
    return int(currentDataRaw) * BATTERY_MONITOR_CURRENT_CONVERSION_FACTOR;
}

int BatteryMonitor::measureBatteryChargeRemaining() {
    // get raw ICA data from byte 4 of page1 (see datasheet page 16):
    updateReadPageData(0x01);
    uint8_t currentAccumulatorDataRaw = pageDataFromDS2438[4];
//    std::cout <<"raw ICA data:  "<< std::bitset<16>( currentAccumulatorDataRaw ) << std::endl;

    // return converted to milliAmps for easier to use:
    return int(currentAccumulatorDataRaw) * BATTERY_MONITOR_CURRENT_ACCUMULATOR_CONVERSION_FACTOR;
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

void BatteryMonitor::setBatteryChargeRemaining(int chargeValueMilliAmpHours){
    // first we must compute the value for the ICA register:
    int temp = int(chargeValueMilliAmpHours / float(BATTERY_MONITOR_CURRENT_ACCUMULATOR_CONVERSION_FACTOR));
    if (temp>0xFF or temp<0){ // not able to store in the 8-bit ICA register, throw error
        throw std::runtime_error("Attempted to set a battery charge remaining value that will not fit in the register.");
    }
    uint8_t newAccumulatorDataRaw = temp & 0xFF; // only 8-bits available

    std::cout <<"new value in mAh:  "<<chargeValueMilliAmpHours<<" = "<< std::bitset<16>( chargeValueMilliAmpHours ) << std::endl;
    std::cout <<"new ICA register:  "<<int(newAccumulatorDataRaw)<<" = "<< std::bitset<8>( newAccumulatorDataRaw ) << std::endl;

    uploadNewByteToDS2438(1,4, newAccumulatorDataRaw);

    int batteryChargeRemaining = measureBatteryChargeRemaining();
    std::cout << "batteryChargeRemaining: " << std::bitset<8>(batteryChargeRemaining) << " = " << batteryChargeRemaining <<"mAh" <<std::endl;

}


void BatteryMonitor::uploadNewDS2438Config(uint8_t statusConfigByte) {
    uploadNewByteToDS2438(0,0,statusConfigByte);
}

void BatteryMonitor::uploadNewByteToDS2438(int pageNumber, int byteNumber, uint8_t newByteValue){

    // make a copy of the current values, so we can re-upload all the bytes we're not changing
    int newPageData [8];
    updateReadPageData(pageNumber); // updates pageDataFromDS2438
    std::copy(std::begin(pageDataFromDS2438), std::end(pageDataFromDS2438), std::begin(newPageData));
    newPageData[byteNumber] = newByteValue; // overwrite the byte to be changed
    std::cout<<"data: "<< std::bitset<8>(newPageData[byteNumber])<<std::endl;

    bool uploadSuccessful = false;
    int retries =0;
    while (!uploadSuccessful) {
        // upload the new status byte to the scratchpad
        reset1Wire();
        skipROM();
        write1WireByte(BATTERY_MONITOR_WRITE_SCRATCHPAD); //write to scratchpad
        write1WireByte(pageNumber); // set the page we want to write to
        int i;
        for(i=0; i<8; i++) { // loop through each byte and write it
            write1WireByte( newPageData[i] );
//            std::cout<<"uploading byte "<<i<<" as "<<std::bitset<8>(newPageData[i])<<std::endl;
        }
        reset1Wire();

        // check the scratchpad values are correct
        updateReadPageDataFromScratchpad(pageNumber);
        uploadSuccessful=true;
        for(i=0; i<8; i++) { // loop through each byte to check it
            if (pageDataFromDS2438[i] != newPageData[i]) {
                // found an error
                uploadSuccessful = false;
                break;
            }
        }

        if (!uploadSuccessful) { // this is bad, try again?
            retries++;
            if (retries > BATTERY_MONITOR_UPLOAD_MAX_RETRIES) { // this is very bad, give up
                std::cout << "debugging information, trying to write: "<<std::endl<<"pageNumber: "<<pageNumber<<std::endl<<", byteNumber: "<<byteNumber<<", value: "<<std::bitset<8>( newByteValue )<<std::endl;
                std::cout << "error on byte number "<<i<<", returned a value of:"<<std::endl<<std::bitset<8>( pageDataFromDS2438[i] )<<std::endl<<"should have been:" <<std::endl << std::bitset<8>( newPageData[i] ) <<std::endl;
                throw std::runtime_error(
                        "Uploading new data to the battery monitor has failed. Check for problems with the 1Wire bus, or bugs in this code!");
            }
        }
    } // end of while (!uploadSuccessful)

    // upload successful to scratchpad, so now need to copy from scratchpad into the actual memory
    reset1Wire();
    skipROM();
    write1WireByte(BATTERY_MONITOR_COPY_SCRATCHPAD_TO_MEMORY); //copy scratchpad to memory
    write1WireByte(pageNumber); //select page 0
    waitFor1WireToFinish();
    reset1Wire();
}

uint16_t BatteryMonitor::measure5VBusVoltage() {
    //check if the battery monitor is currently in bus voltage mode
    updateReadPageData(0x00); // read page0 which contains the status/config byte
    uint8_t statusConfigByte = pageDataFromDS2438[0]; // the 0th byte is the status/config byte

    if ((statusConfigByte & BATTERY_MONITOR_BITMASK_CONFIG_VOLTAGE_AD) != 0){ // if not already in bus voltage mode, we need to change it
        // modify status byte to include battery voltage mode = 0 (for 5V bus measurement)
        statusConfigByte = statusConfigByte & (BATTERY_MONITOR_BITMASK_CONFIG_VOLTAGE_AD^0xF); // keep all bit the same, except the VOLTAGE_AD which becomes 0
        uploadNewDS2438Config(statusConfigByte);
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
        uploadNewDS2438Config(statusConfigByte);
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

    for (int i_page=0; i_page<8; i_page++){
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
