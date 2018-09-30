################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HawkGateway/HawkCrossDomain.cpp \
../src/HawkGateway/HawkGateProxy.cpp \
../src/HawkGateway/HawkGateSecurity.cpp \
../src/HawkGateway/HawkGateThread.cpp \
../src/HawkGateway/HawkGateway.cpp \
../src/HawkGateway/HawkGwControler.cpp 

OBJS += \
./src/HawkGateway/HawkCrossDomain.o \
./src/HawkGateway/HawkGateProxy.o \
./src/HawkGateway/HawkGateSecurity.o \
./src/HawkGateway/HawkGateThread.o \
./src/HawkGateway/HawkGateway.o \
./src/HawkGateway/HawkGwControler.o 

CPP_DEPS += \
./src/HawkGateway/HawkCrossDomain.d \
./src/HawkGateway/HawkGateProxy.d \
./src/HawkGateway/HawkGateSecurity.d \
./src/HawkGateway/HawkGateThread.d \
./src/HawkGateway/HawkGateway.d \
./src/HawkGateway/HawkGwControler.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkGateway/%.o: ../src/HawkGateway/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


