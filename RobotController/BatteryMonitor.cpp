/**
	@file BatteryMonitor.cpp
	@brief Implementation of BatteryMonitor methods
	@author Mike Angus
*/
#include "BatteryMonitor.hpp"

BatteryMonitor::BatteryMonitor() {

}

void BatteryMonitor::direct1WireTest() {
	//Adapted from from https://www.waveshare.com/wiki/Raspberry_Pi_Tutorial_Series:_1-Wire_DS18B20_Sensor
	char path[50] = "/sys/bus/w1/devices/";
    char rom[20];
    char buf[100];
    DIR *dirp;
    struct dirent *direntp;
    int fd =-1;
    float value;
    // These two lines 
	// mount the device:
    system("sudo modprobe w1-gpio");
    system("sudo modprobe w1-therm");
    // Check if /sys/bus/w1/devices/ exists.
    if((dirp = opendir(path)) == NULL)
    {
        printf("opendir error\n");
        return;
    }
    // Reads the directories or files in the current directory.
    while((direntp = readdir(dirp)) != NULL)
    {
        // If 26-00000 is the substring of d_name,
        // then copy d_name to rom and print rom.  
        if(strstr(direntp->d_name,"26-00000"))
        {
            strcpy(rom,direntp->d_name);
            printf(" Rom: %s\n",rom);
        }
    }
    closedir(dirp);
    // Append the String rom and "/vdd" to path
    // path becomes to "/sys/bus/w1/devices/26-00000xxxx/vdd"
    strcat(path,rom);
    strcat(path,"/vdd");
	// Open the file in the path.
    if((fd = open(path,O_RDONLY)) < 0)
    {
        printf("open error\n");
        return;
    }
    // Read the file
    if(read(fd,buf,sizeof(buf)) < 0)
    {
        printf("read error\n");
        return;
    }
    // Read the string in the buffer
    sscanf(buf,"t=%s",buf);
    // atof: changes string to float.
    value = atof(buf)/100;
        printf(" Voltage: %2.2f V\n",value);

    if (value == 0.0f) {
    	printf(" Zero voltage reported, chip may not be connected/responding\n");
    }
}