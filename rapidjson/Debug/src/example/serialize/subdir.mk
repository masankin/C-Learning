################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/example/serialize/serialize.cpp 

OBJS += \
./src/example/serialize/serialize.o 

CPP_DEPS += \
./src/example/serialize/serialize.d 


# Each subdirectory must supply rules for building sources it contributes
src/example/serialize/%.o: ../src/example/serialize/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


