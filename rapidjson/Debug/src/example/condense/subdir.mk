################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/example/condense/condense.cpp 

OBJS += \
./src/example/condense/condense.o 

CPP_DEPS += \
./src/example/condense/condense.d 


# Each subdirectory must supply rules for building sources it contributes
src/example/condense/%.o: ../src/example/condense/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


