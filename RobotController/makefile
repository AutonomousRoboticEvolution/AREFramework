PROG=main
CC=g++
CPPFLAGS=-Wall -I/usr/local/include
LDFLAGS=-L/usr/local/lib -lwiringPi
OBJS=main.o I2CDevice.o MotorOrgan.o SensorOrgan.o Sensors.o BrainOrgan.o

$(PROG) : $(OBJS)
	$(CC) $(CPPFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS)

main.o : main.cpp
	$(CC) $(CPPFLAGS) -c main.cpp
I2CDevice.o : I2CDevice.hpp I2CDevice.cpp
	$(CC) $(CPPFLAGS) -c I2CDevice.cpp
MotorOrgan.o : MotorOrgan.hpp MotorOrgan.cpp
	$(CC) $(CPPFLAGS) -c MotorOrgan.cpp
SensorOrgan.o : SensorOrgan.hpp SensorOrgan.cpp
	$(CC) $(CPPFLAGS) -c SensorOrgan.cpp
Sensors.o : Sensors.hpp Sensors.cpp
	$(CC) $(CPPFLAGS) -c Sensors.cpp
BrainOrgan.o : BrainOrgan.hpp BrainOrgan.cpp
	$(CC) $(CPPFLAGS) -c BrainOrgan.cpp
clean: rm -f core $(PROG) $(OBJS)
