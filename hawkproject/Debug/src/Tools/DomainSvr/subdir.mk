################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Tools/DomainSvr/DomainSvr.cpp 

OBJS += \
./src/Tools/DomainSvr/DomainSvr.o 

CPP_DEPS += \
./src/Tools/DomainSvr/DomainSvr.d 


# Each subdirectory must supply rules for building sources it contributes
src/Tools/DomainSvr/%.o: ../src/Tools/DomainSvr/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


