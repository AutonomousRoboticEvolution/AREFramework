PROG=main
CC=g++
CPPFLAGS=-Wall -I/usr/local/include
LDFLAGS=-L/usr/local/lib -lwiringPi
OBJS=main.o I2CDevice.o Organ.o MotorOrgan.o SensorOrgan.o BrainOrgan.o IMU.o LedDriver.o BatteryMonitor.o Fan.o DaughterBoards.o JointOrgan.o Camera.o

$(PROG) : $(OBJS)
	$(CC) $(CPPFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS)

main.o : main.cpp
	$(CC) $(CPPFLAGS) -c main.cpp
I2CDevice.o : I2CDevice.hpp I2CDevice.cpp
	$(CC) $(CPPFLAGS) -c I2CDevice.cpp
Organ.o : Organ.hpp Organ.cpp
	$(CC) $(CPPFLAGS) -c Organ.cpp
MotorOrgan.o : MotorOrgan.hpp MotorOrgan.cpp
	$(CC) $(CPPFLAGS) -c MotorOrgan.cpp
JointOrgan.o : JointOrgan.hpp JointOrgan.cpp
	$(CC) $(CPPFLAGS) -c JointOrgan.cpp
SensorOrgan.o : SensorOrgan.hpp SensorOrgan.cpp
	$(CC) $(CPPFLAGS) -c SensorOrgan.cpp
BrainOrgan.o : BrainOrgan.hpp BrainOrgan.cpp
	$(CC) $(CPPFLAGS) -c BrainOrgan.cpp
IMU.o : IMU.hpp IMU.cpp
	$(CC) $(CPPFLAGS) -c IMU.cpp
LedDriver.o : LedDriver.hpp LedDriver.cpp
	$(CC) $(CPPFLAGS) -c LedDriver.cpp
BatteryMonitor.o : BatteryMonitor.hpp BatteryMonitor.cpp
	$(CC) $(CPPFLAGS) -c BatteryMonitor.cpp
Fan.o : Fan.hpp Fan.cpp
	$(CC) $(CPPFLAGS) -c Fan.cpp
DaughterBoards.o : DaughterBoards.hpp DaughterBoards.cpp
	$(CC) $(CPPFLAGS) -c DaughterBoards.cpp
Camera.o : Camera.hpp Camera.cpp
	$(CC) $(CPPFLAGS) -c Camera.cpp
clean: rm -f core $(PROG) $(OBJS)
