################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/example/capitalize/capitalize.cpp 

OBJS += \
./src/example/capitalize/capitalize.o 

CPP_DEPS += \
./src/example/capitalize/capitalize.d 


# Each subdirectory must supply rules for building sources it contributes
src/example/capitalize/%.o: ../src/example/capitalize/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


