################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Tools/LogServer/LogServer.cpp 

OBJS += \
./src/Tools/LogServer/LogServer.o 

CPP_DEPS += \
./src/Tools/LogServer/LogServer.d 


# Each subdirectory must supply rules for building sources it contributes
src/Tools/LogServer/%.o: ../src/Tools/LogServer/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


