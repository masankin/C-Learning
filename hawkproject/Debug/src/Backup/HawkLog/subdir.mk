################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Backup/HawkLog/HawkLogProxy.cpp \
../src/Backup/HawkLog/HawkLogServer.cpp 

OBJS += \
./src/Backup/HawkLog/HawkLogProxy.o \
./src/Backup/HawkLog/HawkLogServer.o 

CPP_DEPS += \
./src/Backup/HawkLog/HawkLogProxy.d \
./src/Backup/HawkLog/HawkLogServer.d 


# Each subdirectory must supply rules for building sources it contributes
src/Backup/HawkLog/%.o: ../src/Backup/HawkLog/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


