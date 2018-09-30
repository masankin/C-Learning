################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Tools/NetBridge/NetBridge.cpp 

OBJS += \
./src/Tools/NetBridge/NetBridge.o 

CPP_DEPS += \
./src/Tools/NetBridge/NetBridge.d 


# Each subdirectory must supply rules for building sources it contributes
src/Tools/NetBridge/%.o: ../src/Tools/NetBridge/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


