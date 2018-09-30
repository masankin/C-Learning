################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Samples/TestCase/TestCase.cpp 

OBJS += \
./src/Samples/TestCase/TestCase.o 

CPP_DEPS += \
./src/Samples/TestCase/TestCase.d 


# Each subdirectory must supply rules for building sources it contributes
src/Samples/TestCase/%.o: ../src/Samples/TestCase/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


