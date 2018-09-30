################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/example/simpledom/simpledom.cpp 

OBJS += \
./src/example/simpledom/simpledom.o 

CPP_DEPS += \
./src/example/simpledom/simpledom.d 


# Each subdirectory must supply rules for building sources it contributes
src/example/simpledom/%.o: ../src/example/simpledom/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


