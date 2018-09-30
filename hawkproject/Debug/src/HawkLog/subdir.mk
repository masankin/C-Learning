################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HawkLog/HawkLogProxy.cpp \
../src/HawkLog/HawkLogServer.cpp 

OBJS += \
./src/HawkLog/HawkLogProxy.o \
./src/HawkLog/HawkLogServer.o 

CPP_DEPS += \
./src/HawkLog/HawkLogProxy.d \
./src/HawkLog/HawkLogServer.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkLog/%.o: ../src/HawkLog/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


