################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/example/jsonx/jsonx.cpp 

OBJS += \
./src/example/jsonx/jsonx.o 

CPP_DEPS += \
./src/example/jsonx/jsonx.d 


# Each subdirectory must supply rules for building sources it contributes
src/example/jsonx/%.o: ../src/example/jsonx/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


